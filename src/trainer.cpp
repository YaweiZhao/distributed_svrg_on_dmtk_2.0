#include "trainer.h"
namespace multiverso
{
    namespace svrg
    {
        Trainer::Trainer(int trainer_id, Option *option,
            multiverso::Barrier *barrier, AzsSvrg* AzsSvrg, MemoryManager* memory_mamanger)
        {
            trainer_id_ = trainer_id;
            option_ = option;
            AzsSvrg_ = AzsSvrg;
            barrier_ = barrier;
            memory_mamanger_ = memory_mamanger;
            process_count_ = -1;
            process_id_ = -1;   
            start_ = 0;
            train_count_ = 0;
            //if (trainer_id_ == 0)
            //{
                //The log which recordes the begin and end time of TrainIteration()
            //    char log_name[100];
            //    sprintf(log_name, "trainer_%d.txt", /*g_log_suffix.c_str()*/ multiverso::Multiverso::ProcessRank());
            //    log_file_ = fopen(log_name, "w");
            //}
        }


        void Trainer::TrainIteration(multiverso::DataBlockBase *data_block)
        {
            if (process_id_ == -1)
                process_id_ = multiverso::Multiverso::ProcessRank();

            //if (trainer_id_ == 0)
                //Record the starting time of the Trainiteration  
                //fprintf(log_file_, "%lf\n", (clock()) / (double)CLOCKS_PER_SEC);

            multiverso::Log::Info("Rank %d Train %d Begin TrainIteration%d ...\n",
                process_id_, trainer_id_, train_count_);
            ++train_count_;
            //Compute the total number of processes
            if (process_count_ == -1)
                process_count_ = multiverso::Multiverso::TotalProcessCount();
           
            DataBlock *data = reinterpret_cast<DataBlock*>(data_block);

            //Step 1, Copy the parameter from multiverso to AzsSvrg_
            //One trainer only copy a part of parameters
            multiverso::Log::Debug("Rank %d Train %d Copyparameter Begin TrainIteration%d ...\n",
                process_id_, trainer_id_, train_count_);

	    //    if (trainer_id_ == 0) {
                AzsSvrg_->init(data);
                CopyParameter();
            //}
            multiverso::Log::Debug("Rank %d Train %d Copyparameter end TrainIteration%d ...\n",
                process_id_, trainer_id_, train_count_);
            //Wait for all the trainers to finish copying parameter
            barrier_->Wait();
		
            //Step 2, After finishing copying parameter,
            //Use AzsSvrg_ to train a part of data_block
            clock_t start = clock();
            multiverso::Log::Debug("Rank %d Train %d TrainNN Begin TrainIteration%d ...\n",
                process_id_, trainer_id_, train_count_);
            AzsSvrg_->_train_test();
            //Wait for all the trainers to finish training
            barrier_->Wait();
            multiverso::Log::Debug("Rank %d Train %d AddDeltaParameter Begin TrainIteration%d ...\n",
                process_id_, trainer_id_, train_count_);
            //Step 3, After finishing training, add the delta of parameters to multiverso
            //if (trainer_id_ == 0)
                AddDeltaParameter();
            //barrier_->Wait();

            /*if (trainer_id_ == 0)
            {
                fprintf(log_file_, "%lf\n",
                    (clock()) / (double)CLOCKS_PER_SEC);
                fflush(log_file_);
            }*/
        }

        void Trainer::CopyParameter()
        {
            //Compute the number of necessary memory blocks to store parameter
            size_t total_blocks = option_->class_num;

            //Request blocks to store parameters
            memory_mamanger_->RequestBlocks(total_blocks, blocks);
            assert(blocks.size() == total_blocks);
            if (blocks.size() != total_blocks)
            {
                multiverso::Log::Error("Rank %d Trainer %d Error to requestBlocks to CopyParameter, allocated_blocks_num=%lld, needed_blocks_num=%lld\n",
                    multiverso::Multiverso::ProcessRank(), trainer_id_, blocks.size(), total_blocks);
                return;
            }

            //Copy weights from multiverso to AzsSvrg
            multiverso::Table *table = GetTable(kWeightTableId);

            for (int i = 0; i < option_->class_num; ++i) {
                Row<real>* row = static_cast<Row<real>*>(table->GetRow(i));
                for (int j = 0; j < option_->dim; ++j)
                    blocks[i][j] = row->At(j);
            }

            AzsSvrg_->SetWeight(blocks);
        }

        //Add delta to local buffer and send it to the parameter sever
        void Trainer::AddDeltaParameter()
        {
            std::vector<real*> blocks_get;
            size_t total_blocks = option_->class_num;
            //Request blocks_get to store parameters
            memory_mamanger_->RequestBlocks(total_blocks, blocks_get);
            assert(blocks_get.size() == total_blocks);
            if (blocks_get.size() != total_blocks)
            {
                multiverso::Log::Error("Rank %d Trainer %d Error to requestBlocks to CopyParameter, allocated_blocks_num=%lld, needed_blocks_num=%lld\n",
                    multiverso::Multiverso::ProcessRank(), trainer_id_, blocks_get.size(), total_blocks);
                return;
            }

            AzsSvrg_->GetWeight(blocks_get);

            for (int i = 0; i < option_->class_num; ++i) {
                //multiverso::Row<real>& row = GetRow<real>(kWeightTableId, i);
                for (int j = 0; j < option_->dim; ++j) {
                    //real delta = (blocks_get[i][j] - row.At(j)) / process_count_;
                    real delta = (blocks_get[i][j] - blocks[i][j]) / process_count_;
                    if (fabs(delta) > kEps)
                        Add<real>(kWeightTableId, i, j, delta);
                }
            }

            //Return all the memory blocks
            memory_mamanger_->ReturnBlocks(blocks);
            blocks.clear();
            memory_mamanger_->ReturnBlocks(blocks_get);
            blocks_get.clear();
        }
    }
}
