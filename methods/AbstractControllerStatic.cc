#include <vector>
#include <string>

#include "common.h"

#include "SweepController.h"
#include "ABCRejectionController.h"
#include "ABCSMCController.h"

#include "AbstractController.h"

// Return controller type based on string
controller_t AbstractController::getController(const std::string& arg)
{
    // Check for sweep controller
    if (arg.compare("sweep") == 0)
        return sweep_controller;

    // Check for rejection controller
    else if (arg.compare("rejection") == 0)
        return rejection_controller;

    // Check for smc controller
    else if (arg.compare("smc") == 0)
        return smc_controller;

    // Else return no_controller
    return no_controller;
}

std::string AbstractController::help(controller_t controller)
{
    switch (controller)
    {
        case sweep_controller:
            return SweepController::help();
        case rejection_controller:
            return ABCRejectionController::help();
        case smc_controller:
            return ABCSMCController::help();
        default:
            throw std::runtime_error("Invalid controller type in "
                    "AbstractController::help");
    }
}


AbstractController* AbstractController::makeController(controller_t controller,
        const std::vector<std::string>& positional_args)
{
    switch (controller)
    {
        case sweep_controller:
            return SweepController::makeController(positional_args);
        case rejection_controller:
            return ABCRejectionController::makeController(positional_args);
        case smc_controller:
            return ABCSMCController::makeController(positional_args);
        default:
            throw std::runtime_error("Invalid controller type in "
                    "AbstractController::makeController");
    }
}
