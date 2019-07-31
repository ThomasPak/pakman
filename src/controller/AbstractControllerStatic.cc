#include <vector>
#include <string>

#include "core/common.h"
#include "core/LongOptions.h"
#include "core/Arguments.h"
#include "core/Command.h"

#include "SweepController.h"
#include "ABCRejectionController.h"
#include "ABCSMCController.h"

#include "AbstractController.h"

// Return controller type based on string
controller_t AbstractController::getController(const std::string& arg)
{
    // Check for sweep controller
    if (arg.compare("sweep") == 0)
        return sweep;

    // Check for rejection controller
    else if (arg.compare("rejection") == 0)
        return rejection;

    // Check for smc controller
    else if (arg.compare("smc") == 0)
        return smc;

    // Else return no_controller
    return no_controller;
}

std::string AbstractController::help(controller_t controller)
{
    switch (controller)
    {
        case sweep:
            return SweepController::help();
        case rejection:
            return ABCRejectionController::help();
        case smc:
            return ABCSMCController::help();
        default:
            throw std::runtime_error("Invalid controller type in "
                    "AbstractController::help");
    }
}

void AbstractController::addLongOptions(controller_t controller,
        LongOptions& lopts)
{
    switch (controller)
    {
        case sweep:
            return SweepController::addLongOptions(lopts);
        case rejection:
            return ABCRejectionController::addLongOptions(lopts);
        case smc:
            return ABCSMCController::addLongOptions(lopts);
        default:
            throw std::runtime_error("Invalid controller type in "
                    "AbstractController::makeController");
    }
}

AbstractController* AbstractController::makeController(controller_t controller,
        const Arguments& args)
{
    switch (controller)
    {
        case sweep:
            return SweepController::makeController(args);
        case rejection:
            return ABCRejectionController::makeController(args);
        case smc:
            return ABCSMCController::makeController(args);
        default:
            throw std::runtime_error("Invalid controller type in "
                    "AbstractController::makeController");
    }
}
