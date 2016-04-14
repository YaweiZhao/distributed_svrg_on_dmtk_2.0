#include "parameter_loader.h"

namespace multiverso
{
    namespace svrg
    {
        ParameterLoader::ParameterLoader(Option *option,
            AzsSvrg *AzsSvrg)
        {
            option_ = option;
            AzsSvrg_ = AzsSvrg;

            parse_and_request_count_ = 0;

            //the log which will store the begin and end time of ParseAndRequest
            char log_name[100];
            sprintf(log_name, "parameter_loader%s.txt", g_log_suffix.c_str());
            log_file_ = fopen(log_name, "w");
        }
	
        void ParameterLoader::ParseAndRequest(
            multiverso::DataBlockBase *data_block)
        {
            if (parse_and_request_count_ == 0)
            {
                start_ = clock();
            }

            fprintf(log_file_, "%lf\n", (clock()) / (double)CLOCKS_PER_SEC);
            multiverso::Log::Info("Rank %d ParameterLoader begin %d\n",
                multiverso::Multiverso::ProcessRank(), parse_and_request_count_);
            ++parse_and_request_count_;

            // Request the parameter
            RequestTable(kWeightTableId);           

            fprintf(log_file_, "%lf\n", (clock()) / (double)CLOCKS_PER_SEC);
            fflush(log_file_);
        } 
    }
}
