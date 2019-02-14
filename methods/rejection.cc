#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "common.h"
#include "signal_handler.h"
#include "read_input.h"
#include "run_simulation.h"
#include "write_parameters.h"
#include "types.h"
#include "timer.h"
#include "SerialMaster.h"
#include "ABCRejectionController.h"

#include "debug.h"

bool program_terminated = false;

int main(int argc, char *argv[])
{

    // Set signal handlers
    set_handlers();

    // Process arguments
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " INPUT_FILE NUM_ACCEPT" << std::endl;
        std::cerr << "INPUT_FILE must contain the following lines:\n"
                "EPSILON\n"
                "SIMULATOR\n"
                "PARAMETER_NAMES\n"
                "PRIOR_SAMPLER\n";
        return 2;
    }

    std::ifstream input_file(argv[1]);
    const int num_accept = std::stoi(argv[2]);

    // Extract simulator and parameter specification from standard input
    rejection::input_t input_obj;
    rejection::read_input(input_file, input_obj);

    // Set signal handler
    set_signal_handler();

    // Create SerialMaster and ABC rejection controller
    std::shared_ptr<SerialMaster> p_master =
        std::make_shared<SerialMaster>(input_obj.simulator, &program_terminated);

    std::shared_ptr<ABCRejectionController> p_controller =
        std::make_shared<ABCRejectionController>(input_obj, num_accept);

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

    return 0;
}
