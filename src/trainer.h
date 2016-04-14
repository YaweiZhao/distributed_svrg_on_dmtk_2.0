#pragma once

/*!
* file trainer.h
* \brief Class Trainer trains the model by every trainiteration
*/

#include <thread>
#include <chrono>

#include "multiverso.h"
#include "data_block.h"
#include "constant.h"
#include "util.h"
#include "memory_manager.h"
#include "barrier.h"

#include "AzsSvrg.hpp"

class AzsSvrg;

namespace multiverso
{
    namespace svrg
    {

        extern std::string g_log_suffix;
        class Trainer : public multiverso::TrainerBase
        {
        public:
            int64 word_count;
            Trainer(int trainer_id, Option *option, multiverso::Barrier *barrier, 
                    AzsSvrg* AzsSvrg, MemoryManager* memory_mamanger);
            /*!
            * /brief Train one datablock
            */
            void TrainIteration(multiverso::DataBlockBase* data_block) override;

        private:
            int process_count_;
            int process_id_;
            int trainer_id_;
            Option *option_;
            AzsSvrg* AzsSvrg_;
            multiverso::Barrier *barrier_;
            MemoryManager* memory_mamanger_;
            int train_count_;                   // iteration count, clock
            clock_t start_, now_;
            FILE* log_file_;

            std::vector<real*> blocks;

            /*!
            * \brief Copy the needed parameter from buffer to blocks
            */
            void CopyParameter();
            /*!
            * \brief Add delta to the parameter stored in the 
            * \buffer and send it to multiverso
            */
            void AddDeltaParameter();

            //No copying allowed
            Trainer(const Trainer&);
            void operator=(const Trainer&);
        };
    }
}
