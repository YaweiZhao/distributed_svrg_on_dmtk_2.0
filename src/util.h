#pragma once

/*!
* file util.h
* \brief Struct Option stores many general arguments in model
*/

#include <cstring>
#include <cstdlib>
#include <random>
#include <cassert>
#include <exception>

#include "constant.h"

#include "AzUtil.hpp"
#include "AzParam.hpp"
#include "AzHelp.hpp"

//#include "AzsSvrg.hpp"

namespace multiverso
{
    namespace svrg
    {
        struct Option
        {
            /*---  parameters  ---*/
            bool do_no_intercept, do_dense, do_sparse, do_regress;  
            AzBytArr s_train_x_fn, s_train_tar_fn, s_test_x_fn, s_test_tar_fn; 
            AzBytArr s_param;

            bool is_pipeline;
            int thread_cnt, epoch; 
            int num_servers, num_aggregator, lock_option, num_lock, max_delay;
            //int64 data_block_size;
            //int64 max_preload_data_size;
	        //to do: provide azp.vInt64 interface
            int data_block_size;
            int max_preload_data_size;
            
            int dim;
            int class_num;

            Option();
            /*!
            * \brief Get the model-set arguments from file  
            */
            void parseArgs(const char *inp_param);
            void resetParam(AzParam &azp);
            void printParam(const AzOut &out) const; 
            void printHelp(const AzOut &out) const;
            void throw_if_empty(const char *kw, const AzBytArr &s) const {
                if (s.length() <= 0) throw new AzException(AzInputError, "AzsLinear::resetParam", kw, "must be specified."); 
            }
        };

        std::string GetSystemTime();
        extern std::string g_log_suffix;
    }
}
