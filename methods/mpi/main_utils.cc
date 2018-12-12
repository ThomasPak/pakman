#include "../common.h"
#include "main_utils.h"

worker_t determine_worker_type(bool mpi_simulator, bool persistent_simulator)
{
    if (mpi_simulator)
    {
        if (persistent_simulator)
            return persistent_mpi_worker;
        else
            return mpi_worker;
    }
    else
    {
        if (persistent_simulator)
            return persistent_forked_worker;
        else
            return forked_worker;
    }
}
