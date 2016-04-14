#pragma once

#include "constant.h"

#include "AzUtil.hpp"
#include "AzSmat.hpp"
#include "AzDmat.hpp"
#include "AzSvDataS.hpp"
#include "AzParam.hpp"
#include "AzHelp.hpp"

#include "util.h"

namespace multiverso
{
    namespace svrg
    {
        class Reader
        {
        public:
            Reader(Option *option): option_(option) {}
            ~Reader(){}
            /*!
            * \brief Getsentence from the train_file
            * \param sentence save the sentence by the word index according to the dictionary
            * \param word_count count the sentence length
            */
            void ResetSize(int64 size);

            void GetSamples(AzDSmat *trn_dsm, AzDSmat *tst_dsm, AzDvect *v_trn_y, AzDvect *v_tst_y);

        private:
            const Option *option_;
            FILE* file_;

            int64 byte_count_, byte_size_;

            bool is_dense_matrix(const AzSmat *m) const;

            //No copying allowed
            Reader(const Reader&);
            void operator=(const Reader&);
        };
    }
}
