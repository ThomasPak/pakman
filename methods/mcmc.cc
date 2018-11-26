#include <random>
#include <iostream>
#include <string>
#include <chrono>

#include "read_input.h"
#include "system_call.h"
#include "propose_parameter.h"
#include "acceptance_probability.h"
#include "run_simulation.h"
#include "write_parameters.h"
#include "types.h"
#include "timer.h"

#ifndef NDEBUG
#include "debug.h"
#endif

int main(int argc, char *argv[]) {

#ifndef NDEBUG
    set_handlers();
#endif

    using namespace std;
    using namespace mcmc;

    // Process arguments
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " NUM_ACCEPT" << endl;
        cerr << "Accepts as stdin 7 lines:\n"
                "EPSILON\n"
                "SIMULATOR\n"
                "PARAMETER_NAMES\n"
                "INITIALIZER\n"
                "PROPOSER\n"
                "PRIOR_PDF\n"
                "PROPOSAL_PDF\n";
        return 2;
    }

    const int num_accept = stoi(argv[1]);

    // Seed RNG & initialize distribution
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_real_distribution<double> distribution(0.0, 1.0);

    // Extract simulator and parameter specification from standard input
    input_t input_obj;
    read_input(cin, input_obj);

#ifndef NDEBUG
    cerr << "simulator: " << input_obj.simulator << endl;

    cerr << "Printing names..." << endl;
    for (auto it = input_obj.parameter_names.cbegin();
         it != input_obj.parameter_names.cend(); it++)
        cerr << *it << endl;

    cerr << "initializer: " << input_obj.initializer << endl;
    cerr << "proposer: " << input_obj.proposer << endl;
    cerr << "prior_pdf: " << input_obj.prior_pdf << endl;
    cerr << "proposal_pdf: " << input_obj.proposal_pdf << endl;
#endif

    // Initialize parameter
    parameter_t init_prmtr;
    system_call(input_obj.initializer, init_prmtr);

#ifndef NDEBUG
    cerr << "init_prmtr: " << init_prmtr << endl;
#endif

    // Start main loop
    cerr << "Computing with epsilon = " << input_obj.epsilon << endl;
    start_timer();

    vector<parameter_t> prmtr_accepted;
    prmtr_accepted.push_back(init_prmtr);
    while (prmtr_accepted.size() < num_accept) {

        const parameter_t& prmtr = prmtr_accepted.back();

        // Propose new parameter
        parameter_t new_prmtr;
        propose_parameter(input_obj.proposer, prmtr, new_prmtr);

        // Run simulator
        if (run_simulation(input_obj.epsilon, input_obj.simulator, new_prmtr)) { // Accepted

            // Compute acceptance probability
            double prob = acceptance_probability(
                                        input_obj.prior_pdf, input_obj.proposal_pdf,
                                        prmtr, new_prmtr);

#ifndef NDEBUG
            cerr << "prob: " << prob << endl;
#endif

            // Sample uniformly distributed random number
            double u = distribution(generator);

            // Apply Metropolis rule
            if (u <= prob) {
                prmtr_accepted.push_back(new_prmtr);
                continue;
            }
        }

        // Simulator did not accept or Metropolis rule was not satisfied
        prmtr_accepted.push_back(prmtr);
    }
    //
    // Print time and number of simulations
    stop_timer();
    cerr << "Completed in " << elapsed_time() << " seconds\n";

    // Output accepted parameter values as comma-separated list
    write_parameters(cout, input_obj.parameter_names, prmtr_accepted);

    return 0;
}
