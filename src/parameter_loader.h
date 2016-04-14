#pragma once

/*!
* file parameter_loader.h
* \brief Class Parameterloader parses the datablock and requests the params from multiverso server
*/

#include "multiverso.h"
#include "data_block.h"
#include "constant.h"
#include "util.h"
#include "AzsSvrg.hpp"
#include "log.h"

class AzsSvrg;
namespace multiverso
{
    namespace svrg
    {

        extern std::string g_log_suffix;
		
        class ParameterLoader : public multiverso::ParameterLoaderBase
        {
        public:
            ParameterLoader(){}
            ParameterLoader(Option *option, AzsSvrg *AzsSvrg);
            /*!
            * \brief Parse the datablock to get the parameter needed
            * \param data_block which is pushed in
            */
            void ParseAndRequest(multiverso::DataBlockBase* data_block) override;

        private:
            Option *option_;
            AzsSvrg *AzsSvrg_;
            int parse_and_request_count_;
            clock_t start_;
            FILE* log_file_;

            //No copying allowed
            ParameterLoader(const ParameterLoader&);
            void operator=(const ParameterLoader&);
        };
    }
}
