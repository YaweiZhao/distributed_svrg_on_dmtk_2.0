#include "data_block.h"

namespace multiverso
{
    namespace svrg
    {
        DataBlock::~DataBlock()
        {
            //ClearSamples();
        }

        //size_t DataBlock::Size()
        //{
        //    return samples_.size();
        //}

        //Free the memory of sentences
        //void DataBlock::ClearSamples()
        //{
        //    samples_.clear();
        //}

        void DataBlock::AddSamples(AzDSmat *trn_dsm, AzDSmat *tst_dsm, AzDvect *v_trn_y, AzDvect *v_tst_y)
        {
            this->trn_dsm = trn_dsm;
            this->tst_dsm = tst_dsm;
            this->v_trn_y = v_trn_y;
            this->v_tst_y = v_tst_y;
            //Block block(trn_dsm, tst_dsm, v_trn_y, v_tst_y);
            //samples_.push_back(block);
        }

	    //Get the information of the index-th Samples
        void DataBlock::GetSamples(const AzDSmat **trn_dsm, const AzDSmat **tst_dsm, const AzDvect **v_trn_y, const AzDvect **v_tst_y)
        {
            *trn_dsm = this->trn_dsm;
            *tst_dsm = this->tst_dsm;
            *v_trn_y = this->v_trn_y;
            *v_tst_y = this->v_tst_y;
        }
    }
}
