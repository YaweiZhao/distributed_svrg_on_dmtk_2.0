#pragma once

/*!
* \file constant.h
* \brief The index of parameter tables and some constant.
*/

#include "multiverso.h"
#include "log.h"
#include <cstdint>

namespace multiverso
{
    namespace svrg
    {
        /*! \brief Table id is use*/
        const multiverso::integer_t kWeightTableId = 0;

        typedef int64_t int64;
        typedef uint64_t uint64;
        typedef double real;

        const real kEps = static_cast<real>(1e-10);
    }
}
