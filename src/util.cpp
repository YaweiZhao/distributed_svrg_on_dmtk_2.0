#include "util.h"

namespace multiverso
{
    namespace svrg
    {
        Option::Option()
        {
            do_no_intercept = false;
            do_dense = false;
            do_sparse = false; 
            do_regress =false;  

            is_pipeline = false;
            thread_cnt =1;
            epoch = 1;
            data_block_size = 2000000000;
            max_preload_data_size = 2000000000;

            //multiverso config
            num_servers = 0;
            num_aggregator = 1; 
            lock_option = 1;
            num_lock = 100;
            max_delay = 0;

            class_num = 1;      
        }  

        /*--------------------------------------------------------*/
        #define kw_do_no_intercept "NoIntercept"
        #define kw_train_x_fn "train_x_fn="
        #define kw_train_tar_fn "train_target_fn="
        #define kw_test_x_fn "test_x_fn="
        #define kw_test_tar_fn "test_target_fn="
        #define kw_do_dense "DenseData"
        #define kw_do_sparse "SparseData"
        #define kw_do_regress "Regression"

        #define kw_thread_cnt "threads="
        #define kw_epoch "epoch="
        #define kw_num_servers "num_servers="
        #define kw_num_aggregator "num_aggregator="
        #define kw_lock_option "lock_option="
        #define kw_num_lock "num_lock="
        #define kw_max_delay "max_delay="
        #define kw_data_block_size "data_block_size="
        #define kw_max_preload_data_size "max_preload_data_size="
        #define kw_init_learning_rate "init_learning_rate="
        #define kw_is_pipeline "pipeline"

        #define kw_dim "dim="
        #define kw_class_num "class_num="
        /*--------------------------------------------------------*/
        void Option::resetParam(AzParam &azp)
        {
            azp.swOn(&do_no_intercept, kw_do_no_intercept, false); 
            azp.vStr(kw_train_x_fn, &s_train_x_fn); 
            azp.vStr(kw_test_x_fn, &s_test_x_fn);
            azp.vStr(kw_train_tar_fn, &s_train_tar_fn);
            azp.vStr(kw_test_tar_fn, &s_test_tar_fn);
            azp.swOn(&do_dense, kw_do_dense); 
            azp.swOn(&do_sparse, kw_do_sparse);     
            azp.swOn(&do_regress, kw_do_regress);

            azp.vInt(kw_thread_cnt, &thread_cnt);
            azp.vInt(kw_epoch, &epoch);
            azp.vInt(kw_num_servers, &num_servers);
            azp.vInt(kw_num_aggregator, &num_aggregator);
            azp.vInt(kw_lock_option, &lock_option);
            azp.vInt(kw_num_lock, &num_lock);
            azp.vInt(kw_max_delay, &max_delay);
            azp.vInt(kw_data_block_size, &data_block_size);
            azp.vInt(kw_max_preload_data_size, &max_preload_data_size);
            //azp.vFloat(kw_init_learning_rate, &init_learning_rate);
            azp.swOn(&is_pipeline, kw_is_pipeline);

            azp.vInt(kw_dim, &dim);
            azp.vInt(kw_class_num, &class_num);
        }

        /*--------------------------------------------------------*/
        void Option::printParam(const AzOut &out) const 
        {
            if (out.isNull()) return; 

            AzPrint o(out); 
            o.printSw(kw_do_no_intercept, do_no_intercept); 
            o.printV(kw_train_x_fn, s_train_x_fn); 
            o.printV(kw_train_tar_fn, s_train_tar_fn); 
            o.printV(kw_test_x_fn, s_test_x_fn); 
            o.printV(kw_test_tar_fn, s_test_tar_fn);   
            o.printSw(kw_do_dense, do_dense); 
            o.printSw(kw_do_sparse, do_sparse); 
            o.printSw(kw_do_regress, do_regress);

            o.printV<int>(kw_thread_cnt, thread_cnt);
            o.printV<int>(kw_epoch, epoch);
            o.printV<int>(kw_num_servers, num_servers);
            o.printV<int>(kw_num_aggregator, num_aggregator);
            o.printV<int>(kw_lock_option, lock_option);
            o.printV<int>(kw_num_lock, num_lock);
            o.printV<int>(kw_max_delay, max_delay);
            o.printV<int>(kw_data_block_size, data_block_size);
            o.printV<int>(kw_max_preload_data_size, max_preload_data_size);
            o.printSw(kw_is_pipeline, is_pipeline);

            o.printV<int>(kw_dim, dim);
            o.printV<int>(kw_class_num, class_num);
            o.ppEnd(); 

            throw_if_empty(kw_train_x_fn, s_train_x_fn); 
            throw_if_empty(kw_train_tar_fn, s_train_tar_fn); 
            throw_if_empty(kw_test_x_fn, s_test_x_fn); 
            throw_if_empty(kw_test_tar_fn, s_test_tar_fn);   
            if (do_dense && do_sparse) {
                AzBytArr s(kw_do_sparse); s.c(" and "); s.c(kw_do_dense); s.c(" are mutually exclusive."); 
                throw new AzException(AzInputError, "AzsLinear::resetParam", s.c_str());
            }
        }

        /*------------------------------------------------------------------*/
        void Option::printHelp(const AzOut &out) const
        {
            AzHelp h(out); 
            h.writeln_header("Usage:  linsvrg  parameters | @parameter_filename"); 
            AzBytArr s_kw("  parameters"); 
            AzBytArr s_desc("keyword-value pairs (e.g., \"num_iterations=50\") and options (e.g., \"ShowLoss\") delimited by \",\"."); 
            h.item_noquotes(s_kw.c_str(), s_desc.c_str()); 
            s_kw.reset("  parameter_filename"); 
            s_desc.reset("If the argument begins with \'@\', the rest is regarded as the parameter filename; e.g., if the argument is \"@test.param\", \"test.param\" will be scanned for parameters."); 
            h.item_noquotes(s_kw.c_str(), s_desc.c_str()); 
            h.nl();  
            h.writeln_header("In the parameter description below, \"*\" indicates the required parameters that cannot be omitted.");
            h.nl(); 

            h.item_required(kw_train_x_fn, "Training data feature file.  See README for the format."); 
            h.item_required(kw_train_tar_fn, "Training target file: class labels 0,1,... (classification) or target values (regression); one per line."); 
            h.item_required(kw_test_x_fn, "Test data feature file.  See README for the format."); 
            h.item_required(kw_test_tar_fn, "Test target file: class labels 0,1,... (classification) or target values (regression); one per line.");   
            h.item(kw_do_regress, "Specify this if the task is regression.", " classification task"); 
            h.item(kw_do_no_intercept, "Do not use intercept."); 
            h.item(kw_do_dense, "Use dense matrices for features.", 
                 " automatically determined based on data sparseness"); 
            h.item(kw_do_sparse, "Use sparse matrices for features.", 
                 " automatically determined based on data sparseness"); 
            h.nl(); 

            //AzsSvrg::printHelp(h); 

            h.end();  
        }

        void Option::parseArgs(const char *inp_param)
        {
            const char *param = inp_param; 
            AzBytArr s_tmp; 
            if (*param == '@') {
                /*---  read paramaters from a file  ---*/
                const char *fn = param+1; 
                AzParam::read(param+1, &s_tmp); 
                param = s_tmp.c_str(); 
                cout << param << endl; 
            }
              
            /*---  parse parameters  ---*/
            AzParam azp(param); 
            resetParam(azp); 
            printParam(log_out); 
            azp.check(log_out, &s_param); 
        }

        std::string GetSystemTime()
        {
            time_t t = time(0);
            char tmp[128];
            strftime(tmp, sizeof(tmp), "%Y%m%d%H%M%S", localtime(&t));
            return std::string(tmp);
        }

        std::string g_log_suffix;
    }
}
