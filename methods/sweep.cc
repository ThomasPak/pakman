#include <string>
#include <vector>
#include <iostream>

#include "read_input.h"
#include "run_simulation.h"
#include "write_parameters.h"
#include "types.h"
#include "timer.h"
#include "Sampler.h"

#ifndef NDEBUG
#include "debug.h"
#endif

int main(int argc, char *argv[]) {

#ifndef NDEBUG
    set_handlers();
#endif

    using namespace std;
    using namespace sweep;

    // Process arguments
    if (argc != 1) {
        cerr << "Usage: " << argv[0] << endl;
        cerr << "Accepts as stdin 3 lines:\n"
                "SIMULATOR\n"
                "PARAMETER_NAMES\n"
                "GENERATOR\n";
        return 2;
    }

    // Extract simulator and parameter specification from standard input
    input_t input_obj;
    read_input(cin, input_obj);

    // Initialize Generator
    Generator generator_obj(input_obj.generator);

    // Total number of parameters
    int num_param = generator_obj.getNumberOfParameters();

    // Start main algorithm
    cerr << "Computing " << num_param << " parameters\n";
    start_timer();
    int num_simulated = 0;

    vector<parameter_t> prmtr_simulated;
    while (prmtr_simulated.size() < num_param) {

        // Get generated parameters
        parameter_t prmtr = generator_obj.sampleParameter();

        // Run simulator with dummy epsilon and add to prmtr_simulated
        if (run_simulation("0", input_obj.simulator, prmtr) == 0) {
            std::runtime_error e("simulator rejected parameter");
            throw e;
        }

        prmtr_simulated.push_back(prmtr);

        num_simulated++;
    }

    // Print time and number of simulations
    stop_timer();
    cerr << "Completed in " << elapsed_time() << " seconds\n";

    // Output accepted parameter values as comma-separated list
    write_parameters(cout, input_obj.parameter_names, prmtr_simulated);

    return 0;
}
