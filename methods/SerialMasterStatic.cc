#include <string>

#include <getopt.h>

#include "common.h"
#include "help.h"
#include "signal_handler.h"
#include "debug.h"

#include "SerialMaster.h"

#include <string>
// Static help function
std::string SerialMaster::help()
{
    return
R"(* Help message for 'serial' master *

Description:
  When using a serial master, pakman executes simulations sequentially.  It is
  assumed that the simulator is a standard simulator, which means that it
  communicates with pakman through its stdin and stdout.

Optional arguments:
  -i, --ignore-errors       ignore nonzero return code from simulator
)";
}

// Long options for getopt_long
static struct option const long_options[] =
{
    {"ignore-errors", no_argument, nullptr, 'i'},
    {"help", no_argument, nullptr, 'h'},
    {nullptr, 0, nullptr, 0}
};

// Static run function
void SerialMaster::run(controller_t controller, int argc, char *argv[])
{
    // Process optional arguments
    int c;
    while ((c = getopt_long(argc, argv, "ih", long_options, nullptr)) != -1)
    {
        switch (c)
        {
            case 'i':
                ignore_errors = true;
                break;
            case 'h':
                ::help(serial_master, controller, EXIT_SUCCESS);
            default:
                ::help(serial_master, controller, EXIT_FAILURE);
        }
    }

    // Process positional args
    int argind = optind;
    std::vector<std::string> positional_args;
    while (argv[argind])
        positional_args.push_back(argv[argind++]);

    // Set signal handlers
    set_handlers();
    set_signal_handler();

    // Create controller and SerialMaster
    std::shared_ptr<AbstractController> p_controller(
            AbstractController::makeController(controller, positional_args));

    std::shared_ptr<SerialMaster> p_master =
        std::make_shared<SerialMaster>(p_controller->getSimulator(),
                &program_terminated);

    // Associate with each other
    p_master->assignController(p_controller);
    p_controller->assignMaster(p_master);

    // Start event loop
    while (p_master->isActive())
    {
        p_master->iterate();
    }

    // Destroy Master and Controller
    p_master.reset();
    p_controller.reset();
}
