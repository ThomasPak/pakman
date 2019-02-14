#include <memory>
#include <random>
#include <iostream>
#include <string>
#include <chrono>
#include <fstream>

#include "common.h"
#include "signal_handler.h"
#include "read_input.h"
#include "system_call.h"
#include "run_simulation.h"
#include "write_parameters.h"
#include "types.h"
#include "smc_weight.h"
#include "timer.h"
#include "SerialMaster.h"
#include "ABCSMCController.h"

#include "debug.h"

bool program_terminated = false;

int main(int argc, char *argv[])
{

    // Set signal handlers
    set_handlers();

    // Process arguments
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " INPUT_FILE POPULATION_SIZE" << std::endl;
        std::cerr << "INPUT_FILE must contain the following lines:\n"
                "EPSILONS\n"
                "SIMULATOR\n"
                "PARAMETER_NAMES\n"
                "PRIOR_SAMPLER\n"
                "PERTURBER\n"
                "PRIOR_PDF\n"
                "PERTURBATION_PDF\n";
        return 2;
    }

    std::ifstream input_file(argv[1]);
    const int pop_size = std::stoi(argv[2]);

    // Create random number generator
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    // Extract simulator and parameter specification from standard input
    smc::input_t input_obj;
    smc::read_input(input_file, input_obj);

    // Set signal handler
    set_signal_handler();

    // Create SerialMaster and ABC SMC controller
    std::shared_ptr<SerialMaster> p_master =
        std::make_shared<SerialMaster>(input_obj.simulator, &program_terminated);

    std::shared_ptr<ABCSMCController> p_controller =
        std::make_shared<ABCSMCController>(input_obj, generator, pop_size);

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
