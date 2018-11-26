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
    using namespace rejection;

    // Process arguments
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " NUM_ACCEPT" << endl;
        cerr << "Accepts as stdin 4 lines:\n"
                "EPSILON\n"
                "SIMULATOR\n"
                "PARAMETER_NAMES\n"
                "PRIOR_SAMPLER\n";
        return 2;
    }

    const int num_accept = stoi(argv[1]);
        
    // Extract simulator and parameter specification from standard input
    input_t input_obj;
    read_input(cin, input_obj);

    // Initialize PriorSampler
    PriorSampler prior_sampler_obj(input_obj.prior_sampler);

    // Start main algorithm
    cerr << "Computing with epsilon = " << input_obj.epsilon << endl;
    start_timer();
    int num_simulated = 0;

    vector<parameter_t> prmtr_accepted;
    while (prmtr_accepted.size() < num_accept) {

        // Sample parameter from prior
        parameter_t prmtr_sample = prior_sampler_obj.sampleParameter();

        // Run simulator and add to prmtr_accepted if accepted
        if (run_simulation(input_obj.epsilon, input_obj.simulator, prmtr_sample))
            prmtr_accepted.push_back(prmtr_sample);

        num_simulated++;
    }

    // Print time and number of simulations
    stop_timer();
    cerr << "Completed in " << elapsed_time() << " seconds\n";
    fprintf(stderr, "Accepted/simulated: %d/%d (%5.2f%%)\n", num_accept, num_simulated,
            (100.0 * num_accept / (double) num_simulated));

    // Output accepted parameter values as comma-separated list
    write_parameters(cout, input_obj.parameter_names, prmtr_accepted);

    return 0;
}
