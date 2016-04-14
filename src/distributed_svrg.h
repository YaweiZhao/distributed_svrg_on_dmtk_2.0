#pragma once

/*!
* file distributed_svrg.h
* \brief Class Distributed_svrg describles the main frame of Distributed SVRG and some useful functions 
*/

#include <vector>
#include <ctime>
#include <stdlib.h>
#include <string.h>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>
#include <mutex>
#include <functional>
#include <sys/stat.h>

#include "util.h"
#include "multiverso.h"
#include "data_block.h"
#include "parameter_loader.h"
#include "trainer.h"
#include "reader.h"
#include "log.h"
#include "constant.h"

#include "AzUtil.hpp"

namespace multiverso
{
    namespace svrg
    {
        extern std::string g_log_suffix;
        class Trainer;
        class Distributed_svrg
        {
        public:
            Distributed_svrg(){}
            /*!
            * \brief Run Function contains everything
            */
            void Run(int argc, char *argv[]);

        private:
            clock_t start_;
            int process_id_;
            Option* option_;
            Reader* reader_;
            bool is_running_;
            std::vector<Trainer*> trainers_;
            AzsSvrg *AzsSvrg_;

            /*!
            * \brief Create a new thread which is used for 
            * \calculating the speed of word processing.
            */
            void StartThread();

            /*!
            * \brief Get the file total wordnumber
            */
            int64 GetFileSize(const char *filename);
            /*!
            * \brief Complete the train task with multiverso
            */
            void Train(int argc, char *argv[]);
            void TrainModel();
            /*!
            * \brief Create a new table in the multiverso
            */
            void CreateMultiversoParameterTable(multiverso::integer_t table_id,
                multiverso::integer_t rows, multiverso::integer_t cols,
                multiverso::Type type, multiverso::Format default_format);
            /*!
            * \brief Push the datablock into the multiverso and datablock_queue
            */
            void PushDataBlock(std::queue<DataBlock*> &datablock_queue,
                DataBlock* data_block);
            /*!
            * \brief Prepare parameter table in the multiverso
            */
            void PrepareMultiversoParameterTables();
            /*!
            * \brief Loaddata from train_file to datablock
            * \param datablock the datablock which needs to be assigned
            * \param reader some useful function for calling
            * \param size datablock limit byte size
            */
            void LoadData(DataBlock *data_block, Reader *reader, int64 size);
            /*!
            * \brief Remove datablock which is finished by multiverso thread
            * \param datablock_queue store the pushed datablocks
            */
            void RemoveDoneDataBlock(std::queue<DataBlock*> &datablock_queue);
            // No copying allowed
            Distributed_svrg(const Distributed_svrg&);
            void operator=(const Distributed_svrg&);
        };
    }
}
