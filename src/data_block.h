#pragma once

/*!
* \file data_block.h
* \brief Class DataBlock is to store the necessary data for trainer and param_loader
*/

#include "util.h"
#include "multiverso.h"
#include "constant.h"

#include "AzDmat.hpp"


namespace multiverso
{
    namespace svrg
    {
        /*!
        * \brief The class DataBlock stores train for trainer and param_loader
        */
        class DataBlock : public multiverso::DataBlockBase
        {
        public:
            DataBlock(){}
            ~DataBlock();

            /*!
            * \brief Get the number of sentences stored in DataBlock
            * \return the number of sentences
            */
            //size_t Size();

            /*!
            * \brief Release the memory which are using to store sentences
            */
            //void ClearSamples();

            void AddSamples(AzDSmat *trn_dsm, AzDSmat *tst_dsm, AzDvect *v_trn_y, AzDvect *v_tst_y);
	    void GetSamples(const AzDSmat **trn_dsm, const AzDSmat **tst_dsm, const AzDvect **v_trn_y, const AzDvect **v_tst_y);

        private:

            /*struct Block
            {
            public:
                AzDSmat trn_dsm, tst_dsm; 
                AzDvect v_trn_y, v_tst_y;   
                Block(AzDSmat trn_dsm, AzDSmat tst_dsm, AzDvect v_trn_y, 
                    AzDvect v_tst_y) : trn_dsm(trn_dsm), tst_dsm(tst_dsm),
                    v_trn_y(v_trn_y), v_tst_y(v_tst_y) {}
            };

            std::vector<Block> samples_;*/

            AzDSmat *trn_dsm, *tst_dsm; 
            AzDvect *v_trn_y, *v_tst_y;

            // No copying allowed
            DataBlock(const DataBlock&);
            void operator=(const DataBlock&);
        };
    }
}
