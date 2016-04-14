#define _AZ_MAIN_

#include <thread>
#include <string>
#include <iostream>
#include <cstring>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <new>

#include "util.h"
#include "reader.h"
#include "multiverso.h"
#include "barrier.h"
#include "distributed_svrg.h"
#include "parameter_loader.h"
#include "trainer.h"
#include "memory_manager.h"

#include "AzUtil.hpp"
#include "AzsSvrg.hpp"

using namespace multiverso;
using namespace svrg;
int main(int argc, char *argv[])
{   
    try
    {
        Distributed_svrg *ptr = new (std::nothrow)Distributed_svrg();
        assert(ptr != nullptr);
        ptr->Run(argc, argv);
    }
    catch (std::bad_alloc &memExp)
    {
        multiverso::Log::Info("Something wrong with new() %s\n", memExp.what());
    }
    catch(...)
    {
        multiverso::Log::Info("Something wrong with other reason!\n");
    }
    return 0;
}
