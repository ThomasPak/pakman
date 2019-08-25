#include <string>
#include <memory>

#include <getopt.h>

#include "core/common.h"
#include "core/Arguments.h"
#include "core/LongOptions.h"
#include "core/Command.h"
#include "system/signal_handler.h"
#include "system/debug.h"
#include "controller/AbstractController.h"

#include "SerialMaster.h"

// Static help function
std::string SerialMaster::help()
{
    return
R"(* Help message for 'serial' master *

Description:
  When using a serial master, pakman executes simulations sequentially.  It is
  assumed that the simulator is a standard simulator, which means that it
  communicates with pakman through its stdin and stdout.
)";
}

void SerialMaster::addLongOptions(LongOptions& lopts)
{
    // No SerialMaster-specific options to add
}

// Static run function
void SerialMaster::run(controller_t controller, const Arguments& args)
{
    // Set signal handlers
    set_handlers();
    set_signal_handler();

    // Create controller and SerialMaster
    std::shared_ptr<AbstractController>
        p_controller(AbstractController::makeController(controller, args));

    auto p_master =
        std::make_shared<SerialMaster>(p_controller->getSimulator(),
                &g_program_terminated);

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

// Static cleanup function
void SerialMaster::cleanup()
{
}
