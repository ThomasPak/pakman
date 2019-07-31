#include <string>

#include "core/common.h"
#include "core/LongOptions.h"
#include "core/Arguments.h"
#include "core/Command.h"

#include "SerialMaster.h"
#include "MPIMaster.h"

#include "AbstractMaster.h"

// Return master type based on string
master_t AbstractMaster::getMaster(const std::string& arg)
{
    // Check for serial master
    if (arg.compare("serial") == 0)
        return serial;

    // Check for mpi master
    else if (arg.compare("mpi") == 0)
        return mpi;

    // Else return no_master
    return no_master;
}

std::string AbstractMaster::help(master_t master)
{
    switch (master)
    {
        case serial:
            return SerialMaster::help();
        case mpi:
            return MPIMaster::help();
        default:
            throw std::runtime_error(
                    "Invalid master type in "
                    "AbstractMaster::help");
    }
}

void AbstractMaster::addLongOptions(master_t master,
        LongOptions& lopts)
{
    switch (master)
    {
        case serial:
            SerialMaster::addLongOptions(lopts);
            return;
        case mpi:
            MPIMaster::addLongOptions(lopts);
            return;
        default:
            throw std::runtime_error(
                    "Invalid master type in "
                    "AbstractMaster::run");
    }
}

void AbstractMaster::run(master_t master, controller_t controller,
        const Arguments& args)
{
    switch (master)
    {
        case serial:
            SerialMaster::run(controller, args);
            return;
        case mpi:
            MPIMaster::run(controller, args);
            return;
        default:
            throw std::runtime_error(
                    "Invalid master type in "
                    "AbstractMaster::run");
    }
}

void AbstractMaster::cleanup(master_t master)
{
    switch (master)
    {
        case serial:
            SerialMaster::cleanup();
            return;
        case mpi:
            MPIMaster::cleanup();
            return;
        default:
            throw std::runtime_error(
                    "Invalid master type in "
                    "AbstractMaster::cleanup");
    }
}
