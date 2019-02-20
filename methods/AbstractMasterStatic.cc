#include <string>

#include "common.h"

#include "SerialMaster.h"
#include "MPIMaster.h"

#include "AbstractMaster.h"

// Return master type based on string
master_t AbstractMaster::getMaster(const std::string& arg)
{
    // Check for serial master
    if (arg.compare("serial") == 0)
        return serial_master;

    // Check for mpi master
    else if (arg.compare("mpi") == 0)
        return mpi_master;

    // Else return no_master
    return no_master;
}

std::string AbstractMaster::help(master_t master)
{
    switch (master)
    {
        case serial_master:
            return SerialMaster::help();
        case mpi_master:
            return MPIMaster::help();
        default:
            throw std::runtime_error(
                    "Invalid master type in "
                    "AbstractMaster::help");
    }
}

void AbstractMaster::run(master_t master, controller_t controller,
        int argc, char *argv[])
{
    switch (master)
    {
        case serial_master:
            SerialMaster::run(controller, argc, argv);
            return;
        case mpi_master:
            MPIMaster::run(controller, argc, argv);
            return;
        default:
            throw std::runtime_error(
                    "Invalid master type in "
                    "AbstractMaster::run");
    }
}
