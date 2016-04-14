#include "distributed_svrg.h"

namespace multiverso
{
    namespace svrg
    {
        void Distributed_svrg::Run(int argc, char *argv[])
        {
            g_log_suffix = GetSystemTime();
            srand(static_cast<unsigned int>(time(NULL)));
            option_ = new (std::nothrow)Option();
            assert(option_ != nullptr);
            //Parse argument and store them in option
            if (argc < 2) {
                option_->printHelp(log_out); 
                return; 
            }
            option_->parseArgs(argv[1]);
            reader_ = new (std::nothrow)Reader(option_);
            this->Train(argc, argv);

            delete option_;
            delete reader_;
        }

        void Distributed_svrg::Train(int argc, char *argv[])
        {
            //The barrier for trainers
            multiverso::Barrier* barrier =
            new multiverso::Barrier(option_->thread_cnt);

            MemoryManager* memory_mamanger = new MemoryManager(option_->dim);
            AzsSvrg* AzsSvrgs[2] = {new AzsSvrg(option_), new AzsSvrg(option_)};

            //Step 1, Create Multiverso ParameterLoader and Trainers, 

            //Prepare option_->thread_cnt trainers for multiverso
            std::vector<multiverso::TrainerBase*> trainers;
            for (int i = 0; i < option_->thread_cnt; ++i)
            {
                trainers.push_back(new (std::nothrow)Trainer(i, option_,
                    barrier, AzsSvrgs[1], memory_mamanger));
                assert(trainers[i] != nullptr);
            }

            //Prepare ParameterLoader
            ParameterLoader *parameter_loader =new (std::nothrow)ParameterLoader(
                option_, AzsSvrgs[0]);
            assert(parameter_loader != nullptr);

            //Step 2, prepare the Config for multiverso
            multiverso::Config config;
            config.max_delay = option_->max_delay;
            config.num_servers = option_->num_servers;
            config.num_aggregator = option_->num_aggregator;
            config.is_pipeline = option_->is_pipeline;
            config.lock_option = static_cast<multiverso::LockOption>(option_->lock_option);
            config.num_lock = option_->num_lock;

            //Step3, Init the environment of multiverso
            multiverso::Multiverso::Init(trainers, parameter_loader, config, &argc, &argv);

            char log_name[100];
            sprintf(log_name, "log%s.txt", g_log_suffix.c_str());
            multiverso::Log::ResetLogFile(log_name);

            //Mark the node machine number
            process_id_ = multiverso::Multiverso::ProcessRank();

            //Step 4, prepare the sever/aggregator/cache Table for parametertable(3 or 5) 
            //and initialize the severtable for inputvector
            PrepareMultiversoParameterTables();
            
            //Step 5, start the Train of NN
            TrainModel();

            delete barrier;
            delete memory_mamanger;
            delete AzsSvrgs[0];
            delete AzsSvrgs[1];
            for (auto trainer : trainers)
            {
                delete trainer;
            }
            delete parameter_loader;
            multiverso::Multiverso::Close();
        }

        void Distributed_svrg::PrepareMultiversoParameterTables()
        {
            multiverso::Multiverso::BeginConfig();
            int proc_count = multiverso::Multiverso::TotalProcessCount();

            //Create tables, the order of creating tables should arise from 0 continuously
            //The elements of talbes will be initialized with 0
            CreateMultiversoParameterTable(kWeightTableId,
                option_->class_num, option_->dim,
                multiverso::Type::Double, multiverso::Format::Dense);

            multiverso::Multiverso::EndConfig();
        }

        //Create the three kinds of tables
        void Distributed_svrg::CreateMultiversoParameterTable(
            multiverso::integer_t table_id, multiverso::integer_t rows,
            multiverso::integer_t cols, multiverso::Type type,
            multiverso::Format default_format)
        {
            multiverso::Multiverso::AddServerTable(table_id, rows,
                cols, type, default_format);
            multiverso::Multiverso::AddCacheTable(table_id, rows,
                cols, type, default_format, 0);
            multiverso::Multiverso::AddAggregatorTable(table_id, rows,
                cols, type, default_format, 0);
        }

        //Get the size of filename, it should deal with large files
        int64 Distributed_svrg::GetFileSize(const char *filename)
        {
#ifdef _MSC_VER
            struct _stat64 info;
            _stat64(filename, &info);
            return (int64)info.st_size;
#else
            struct  stat info;
            stat(filename, &info);
            return(int64)info.st_size;
#endif  
        }

        void Distributed_svrg::TrainModel()
        {
            std::queue<DataBlock*> datablock_queue;
            int data_block_count = 0;

            char file_x[200];
            sprintf(file_x, "%s_%d", option_->s_train_x_fn.c_str(), process_id_); 
            int64 file_size = GetFileSize(file_x);
            multiverso::Log::Info("train-file-size:%lld, data_block_size:%lld\n",
                file_size, option_->data_block_size);
            start_ = clock();
            multiverso::Multiverso::BeginTrain();
            for (int cur_epoch = 0; cur_epoch < option_->epoch; ++cur_epoch)
            {
                //reader_->ResetStart();
                multiverso::Multiverso::BeginClock();
                for (int64 cur = 0; cur < file_size; cur += option_->data_block_size)   
                {
                    ++data_block_count;
                    DataBlock *data_block = new (std::nothrow)DataBlock();
                    assert(data_block != nullptr);
                    //Load the sentences from train file, and store them in data_block
                    clock_t start = clock();
                    LoadData(data_block, reader_, option_->data_block_size);
                    multiverso::Log::Info("LoadOneDataBlockTime:%lfs\n",
                        (clock() - start) / (double)CLOCKS_PER_SEC);
                    PushDataBlock(datablock_queue, data_block);
                }
                multiverso::Multiverso::EndClock();
            }

            multiverso::Log::Info("Rank %d Pushed %d datablocks\n", process_id_, data_block_count);

            multiverso::Multiverso::EndTrain();

            //After EndTrain, all the datablock are done,
            //we remove all the datablocks
            RemoveDoneDataBlock(datablock_queue);
        }
              
        void Distributed_svrg::LoadData(DataBlock *data_block, Reader *reader, int64 size)
        {
            //Be sure to clear all the sentences
            //which were stored in data_block
            //data_block->ClearSamples();
            reader->ResetSize(size);
            /*while (true)
            {
                int64 word_count = 0;
                int *sentence = new (std::nothrow)int[kMaxSentenceLength + 2];
                assert(sentence != nullptr);
                int sentence_length = reader->GetSentence(sentence, word_count);
                if (sentence_length > 0)
                {
                    data_block->AddSentence(sentence, sentence_length,
                        word_count, (uint64)rand() * 10000 + (uint64)rand());
                }
                else
                {
                    //Reader read eof or has read data_block->size bytes before,
                    //reader_->GetSentence will return 0
                    delete[] sentence;
                    break;
                }
            }*/
            AzDSmat *trn_dsm, *tst_dsm;
            AzDvect *v_trn_y, *v_tst_y;
            trn_dsm = new AzDSmat();
            tst_dsm = new AzDSmat();
            v_trn_y = new AzDvect();
            v_tst_y = new AzDvect();
            reader->GetSamples(trn_dsm, tst_dsm, v_trn_y, v_tst_y);
            data_block->AddSamples(trn_dsm, tst_dsm, v_trn_y, v_tst_y);
        }

        void Distributed_svrg::PushDataBlock(
            std::queue<DataBlock*> &datablock_queue, DataBlock* data_block)
        {
            
            multiverso::Multiverso::PushDataBlock(data_block);
            
            datablock_queue.push(data_block);
            //limit the max size of total datablocks to avoid out of memory
            while (static_cast<int64>(datablock_queue.size()) * option_->data_block_size
                > option_->max_preload_data_size)
            {
                std::chrono::milliseconds dura(200);
                std::this_thread::sleep_for(dura);
                //Remove the datablock which is delt by parameterloader and trainer
                RemoveDoneDataBlock(datablock_queue);
            }
        }

        //Remove the datablock which is delt by parameterloader and trainer
        void Distributed_svrg::RemoveDoneDataBlock(
            std::queue<DataBlock*> &datablock_queue)
        {
            while (datablock_queue.empty() == false 
                && datablock_queue.front()->IsDone())
            {
                DataBlock *p_data_block = datablock_queue.front();
                datablock_queue.pop();
                delete p_data_block;
            }
        }

    }
}

