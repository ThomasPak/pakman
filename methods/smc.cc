#include <random>
#include <iostream>
#include <string>
#include <chrono>

#include "read_input.h"
#include "system_call.h"
#include "run_simulation.h"
#include "write_parameters.h"
#include "types.h"
#include "smc_weight.h"
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
    using namespace smc;

    // Process arguments
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " POPULATION_SIZE" << endl;
        cerr << "Accepts as stdin 7 lines:\n"
                "EPSILONS\n"
                "SIMULATOR\n"
                "PARAMETER_NAMES\n"
                "PRIOR_SAMPLER\n"
                "PERTURBER\n"
                "PRIOR_PDF\n"
                "PERTURBATION_PDF\n";
        return 2;
    }

    const int pop_size = stoi(argv[1]);

    // Seed RNG
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);

    // Extract simulator and parameter specification from standard input
    input_t input_obj;
    read_input(cin, input_obj);

#ifndef NDEBUG
    cerr << "Printing epsilons..." << endl;
    for (auto it = input_obj.epsilons.cbegin();
         it != input_obj.epsilons.cend(); it++)
        cerr << *it << endl;

    cerr << "simulator: " << input_obj.simulator << endl;

    cerr << "Printing names..." << endl;
    for (auto it = input_obj.parameter_names.cbegin();
         it != input_obj.parameter_names.cend(); it++)
        cerr << *it << endl;

    cerr << "prior_sampler: " << input_obj.prior_sampler << endl;
    cerr << "perturber: " << input_obj.perturber << endl;
    cerr << "prior_pdf: " << input_obj.prior_pdf << endl;
    cerr << "perturbation_pdf: " << input_obj.perturbation_pdf << endl;
#endif

    // Initialize smc_sampler
    SMCSampler smc_sampler((vector<double>(pop_size)),
                           (vector<parameter_t>(pop_size)), generator, input_obj.perturber,
                           input_obj.prior_sampler,
                           input_obj.prior_pdf);

    // Initialize vector to store new weights and parameters
    vector<parameter_t> prmtr_accepted_new(pop_size);
    vector<double> weights_new(pop_size);

    // Main loop
    for (int t = 0; t < input_obj.epsilons.size(); t++) {

        // Print message
        cerr << "Computing generation " << t << ", epsilon = " << input_obj.epsilons[t] << endl;
        start_timer();

        // Set t in smc_sampler
        smc_sampler.setT(t);

        // Get current epsilon
        const string epsilon(input_obj.epsilons[t]);

        // Initialize counters
        int num_accepted = 0, num_simulated = 0;

        while (num_accepted < pop_size) {

            // Sample parameter from previous population and perturb
            parameter_t prmtr_perturbed = smc_sampler.sampleParameter();

            // Run simulator
            if (run_simulation(epsilon, input_obj.simulator, prmtr_perturbed)) { // Accepted

                // Add accepted parameter
                prmtr_accepted_new[num_accepted] = prmtr_perturbed;

                // Calculate weight
                weights_new[num_accepted] = smc_weight(input_obj.perturbation_pdf,
                                                   smc_sampler.getPriorPdf(),
                                                   t,
                                                   smc_sampler.getParameterPopulation(),
                                                   smc_sampler.getWeights(),
                                                   prmtr_perturbed);
                // Increment accepted parameter counter
                num_accepted++;
            }

            // Increment simulation counter
            num_simulated++;
        }

        // Swap population and weights
        smc_sampler.swap_population(weights_new, prmtr_accepted_new);

        // Print time and number of simulations
        stop_timer();
        cerr << "Completed in " << elapsed_time() << " seconds\n";
        fprintf(stderr, "Accepted/simulated: %d/%d (%5.2f%%)\n", num_accepted, num_simulated,
                (100.0 * num_accepted / (double) num_simulated));

#ifndef NDEBUG
        // Output accepted parameter values as comma-separated list
        write_parameters(cerr, input_obj.parameter_names, smc_sampler.getParameterPopulation());

        cerr << "Printing weights..." << endl;
        for (auto it = smc_sampler.getWeights().cbegin();
             it != smc_sampler.getWeights().cend(); it++)
            cerr << *it << endl;
#endif

    }

    // Output accepted parameter values as comma-separated list
    write_parameters(cout, input_obj.parameter_names, smc_sampler.getParameterPopulation());

    return 0;
}
