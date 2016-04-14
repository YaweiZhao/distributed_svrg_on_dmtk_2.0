#include "reader.h"

namespace multiverso
{
    namespace svrg
    {
        void Reader::ResetSize(int64 size)
        {
            byte_count_ = 0;
            byte_size_ = size;
        }      

        void Reader::GetSamples(AzDSmat &trn_dsm, AzDSmat &tst_dsm, AzDvect &v_trn_y, AzDvect &v_tst_y) 
        {      
            /*---  read data  ---*/           
            AzSvDataS trn_ds, tst_ds; 
            AzTimeLog::print("Reading training data ... ", log_out); 
            trn_ds.read(option_->s_train_x_fn.c_str(), option_->s_train_tar_fn.c_str()); 
            AzTimeLog::print("Reading test data ... ", log_out); 
            tst_ds.read(option_->s_test_x_fn.c_str(), option_->s_test_tar_fn.c_str());

            if (!option_->do_no_intercept) { /* append a feature with a constant 1 */
                AzTimeLog::print("Adding a constant feature ...", log_out); 
                trn_ds.append_const(1); 
                tst_ds.append_const(1); 
            }
            v_trn_y = new AzDvect(trn_ds.targets());
            v_tst_y = new AzDvect(tst_ds.targets()); 
            AzBytArr s_num("#train="); s_num.cn(trn_ds.size()); s_num.c(", #test="); s_num.cn(tst_ds.size()); 
      
            /*---  sparse?  dense?  ---*/
            AzDmat md_trn, md_tst; 
            if (is_dense_matrix(trn_ds.feat())) {
                /*---  treat it as dense data  ---*/
                AzTimeLog::print("Processing as DENSE data ... ", s_num.c_str(), log_out); 
                md_trn.set(trn_ds.feat()); 
                trn_ds.destroy(); 
                trn_dsm.reset(&md_trn); 
                md_tst.set(tst_ds.feat()); 
                tst_ds.destroy(); 
                tst_dsm.reset(&md_tst); 
            }
            else {
                /*---  treat it as sparse data  ---*/
                AzTimeLog::print("Processing as SPARSE data ... ", s_num.c_str(), log_out); 
                trn_dsm.reset(trn_ds.feat()); 
                tst_dsm.reset(tst_ds.feat()); 
            }
        }

        /*--------------------------------------------------------*/
        bool Reader::is_dense_matrix(const AzSmat *m) const
        {
            if (option_->do_dense) return true; 
            if (option_->do_sparse) return false; 
            AzTimeLog::print("Checking data sparsity ... ", log_out); 
            double nz_ratio; 
            m->nonZeroNum(&nz_ratio); 
            AzBytArr s("nonzero ratio = "); s.cn(nz_ratio); 
            AzPrint::writeln(log_out, s); 
            return (nz_ratio >= 0.6666); 
        }
    }
}
