#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <set>
#include <thread>
#include <random>
#include <chrono>
#include <stdexcept>
#include <mpi.h>

#include "../types.h"
#include "../read_input.h"
#include "../system_call.h"
#include "../write_parameters.h"
#include "../run_simulation.h"
#include "../Sampler.h"
#include "../smc_weight.h"
#include "mpi_utils.h"
#include "common.h"
#include "master.h"
#include "../timer.h"

void check_managers(const std::vector<ParameterHandler*>& manager_map,
                    std::set<int>& idle_managers) {

    // While there are any incoming messages
    MPI::Status status;
    while (MPI::COMM_WORLD.Iprobe(MPI_ANY_SOURCE, OUTPUT_TAG, status)) {

        // Receive message
        int msg, manager = status.Get_source();
        MPI::COMM_WORLD.Recv(&msg, 1, MPI::INT, manager, OUTPUT_TAG);

#ifndef NDEBUG
        const int rank = MPI::COMM_WORLD.Get_rank();
        const int size = MPI::COMM_WORLD.Get_size();
        std::cerr << "Master " << rank << "/" << size
                  << ": received result message from manager "
                  << manager << std::endl;
#endif

        // Set parameter status appropriately according to message received
        switch (msg) {
            case REJECT:
                if (!tolerate_rejections) {
                    std::runtime_error e("Simulator rejected simulation!");
                    throw e;
                }
                manager_map[manager]->setStatus(rejected);
                break;
            case ACCEPT:
                manager_map[manager]->setStatus(accepted);
                break;
            case ERROR:
                if (!tolerate_errors) {
                    std::runtime_error e("Simulator returned error!");
                    throw e;
                }
                manager_map[manager]->setStatus(error);
                break;
            case CANCEL:
                manager_map[manager]->setStatus(cancelled);
                break;
        }

        // Mark manager as idle
        idle_managers.insert(manager);
    }
}

void delegate_managers(const AbstractSampler& sampler_obj,
                       const std::string& epsilon,
                       std::vector<ParameterHandler*>& manager_map,
                       std::set<int>& idle_managers,
                       std::queue<ParameterHandler>& prmtr_sampled) {

    // Loop over idle managers
    for (auto it = idle_managers.begin();
         it != idle_managers.end(); it++) {

        // Sample parameter and push to queue
        prmtr_sampled.emplace(sampler_obj.sampleParameter());

        // Format input message
        std::string input_str =
            simulator_input(epsilon, prmtr_sampled.back().getCString());

        // Send input message to manager
        MPI::COMM_WORLD.Isend(input_str.c_str(),
                              input_str.size() + 1,
                              MPI::CHAR, *it, INPUT_TAG);

#ifndef NDEBUG
        const int rank = MPI::COMM_WORLD.Get_rank();
        const int size = MPI::COMM_WORLD.Get_size();
        std::cerr << "Master " << rank << "/" << size
                  << ": sent parameter message to manager "
                  << *it << std::endl;
#endif

        // Set manager mapping
        manager_map[*it] = &prmtr_sampled.back();
    }

    // Clear idle managers
    idle_managers.clear();
}

void check_parameters(std::queue<ParameterHandler>& prmtr_sampled,
                      std::vector<parameter_t>& prmtr_accepted) {

    // Check if front parameter has finished simulating
    while ( !prmtr_sampled.empty()
         && (prmtr_sampled.front().getStatus() != busy) ) {

#ifndef NDEBUG
        std::cerr << "Master: checking parameters\n"
            << "Printing queue...\n";
        std::queue<ParameterHandler> prmtr_sampled_copy(prmtr_sampled);
        while (!prmtr_sampled_copy.empty()) {
            std::cerr << prmtr_sampled_copy.front().getStatus() << " ";
            prmtr_sampled_copy.pop();
        }
        std::cerr << std::endl;
#endif

        // Push to accepted parameters if accepted
        if (prmtr_sampled.front().getStatus() == accepted)
            prmtr_accepted.push_back(prmtr_sampled.front().getParameter());

        // Sanity check: do not check cancelled parameters
        if (prmtr_sampled.front().getStatus() == cancelled) {
            std::runtime_error e("cannot check cancelled parameters");
            throw e;
        }

        // Iterate and pop front parameter
        prmtr_sampled.pop();
    }
}

void send_signal_to_managers(const int signal) {

    const int size = MPI::COMM_WORLD.Get_size();

    std::vector<MPI::Request> reqs;

    for (int manager_idx = 0; manager_idx < size; manager_idx++)
        reqs.push_back(MPI::COMM_WORLD.Isend(&signal, 1, MPI::INT, manager_idx, SIGNAL_TAG));

    for (auto& req : reqs) req.Wait();
}

void send_signal_to_idle_managers(const int signal,
        std::set<int>& idle_managers) {

    std::vector<MPI::Request> reqs;

    for (auto it = idle_managers.begin();
         it != idle_managers.end(); it++)
        reqs.push_back(MPI::COMM_WORLD.Isend(&signal, 1, MPI::INT, *it, SIGNAL_TAG));

    for (auto& req : reqs) req.Wait();
}

namespace rejection {

void master(const int num_accept, const input_t& input_obj) {

    // Get size
    const int size = MPI::COMM_WORLD.Get_size();

    // Create prior sampler object
    PriorSampler prior_sampler_obj(input_obj.prior_sampler);

    // Initialize idle managers set
    std::set<int> idle_managers;
    for (int i = 0; i < size; i++)
        idle_managers.insert(i);

    // Initialize queue to hold sampled parameter handlers
    std::queue<ParameterHandler> prmtr_sampled;

    // Initialize manager to parameter handler map
    std::vector<ParameterHandler*> manager_map(size);

    // Initialize prmtr_accepted
    std::vector<parameter_t> prmtr_accepted;

    // Start main loop
    for (;;) {

        // Check if any managers have finished their work
        check_managers(manager_map, idle_managers);

        // Send work to idle managers
        delegate_managers(prior_sampler_obj, input_obj.epsilon, manager_map,
                idle_managers, prmtr_sampled);

        // Add parameter to prmtr_accepted in the order they were sampled
        check_parameters(prmtr_sampled, prmtr_accepted);

        // If we have enough parameters
        if (prmtr_accepted.size() >= num_accept) {

            // Terminate all managers
            send_signal_to_managers(TERMINATE_MANAGER_SIGNAL);

#ifndef NDEBUG
            // Print if there are any superfluous parameters
            if (prmtr_accepted.size() > num_accept)
                std::cerr << "There were " << (prmtr_accepted.size() - num_accept)
                          << " too many parameters sampled\n";
#endif

            // Pop any superfluous parameters
            while (prmtr_accepted.size() > num_accept)
                prmtr_accepted.pop_back();

            // Output accepted parameter values as comma-separated list
            write_parameters(std::cout, input_obj.parameter_names, prmtr_accepted);

            // Return
            return;
        }

        // If terminate_program is set, return
        if (terminate_program) return;

        // Sleep for MAIN_TIMEOUT
        std::this_thread::sleep_for(MAIN_TIMEOUT);
    }
}

}

namespace mcmc {

void master() {

    // Get rank and size
    const int size = MPI::COMM_WORLD.Get_size();
    const int rank = MPI::COMM_WORLD.Get_rank();

    std::cerr << "Master: rank " << rank << "/" << size << std::endl;
}

}

namespace smc {

void compute_weights(const SMCSampler& smc_sampler,
                     const cmd_t& perturbation_pdf,
                     const int t,
                     const std::vector<parameter_t>& prmtr_accepted_new,
                     std::vector<double>& weights_new) {

    // Iterate over parameters whose weights have not yet been computed
    for (int i = weights_new.size(); i < prmtr_accepted_new.size(); i++)
        weights_new.push_back(smc_weight(
                perturbation_pdf,
                smc_sampler.getPriorPdf(),
                t,
                smc_sampler.getParameterPopulation(),
                smc_sampler.getWeights(),
                prmtr_accepted_new[i]));
}

void master(const int pop_size, const input_t& input_obj) {

    // Get size
    const int size = MPI::COMM_WORLD.Get_size();

    // Seed RNG
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    // Initialize smc_sampler
    SMCSampler smc_sampler(
            (std::vector<double>(pop_size)),
            (std::vector<parameter_t>(pop_size)), generator, input_obj.perturber,
            input_obj.prior_sampler,
            input_obj.prior_pdf);

    // Initialize vector to store new weights and parameters
    std::vector<parameter_t> prmtr_accepted_new;
    std::vector<double> weights_new;

    // Main loop
    for (int t = 0; t < input_obj.epsilons.size(); t++) {

        // Print message
        std::cerr << "Computing generation " << t <<
            ", epsilon = " << input_obj.epsilons[t] << std::endl;
        start_timer();

        // Set t
        smc_sampler.setT(t);

        // Initialize counter
        int num_simulated = 0;

        // Initialize idle managers set
        std::set<int> idle_managers;
        for (int i = 0; i < size; i++)
            idle_managers.insert(i);

        // Initialize queue to hold sampled parameter handlers
        std::queue<ParameterHandler> prmtr_sampled;

        // Initialize manager to parameter handler map
        std::vector<ParameterHandler*> manager_map(size);

        // Start control loop
        for (;;) {

            // Check if any managers have finished their work
            check_managers(manager_map, idle_managers);

            // Send work to idle managers
            delegate_managers(smc_sampler, input_obj.epsilons[t], manager_map,
                    idle_managers, prmtr_sampled);

            // Add parameter to prmtr_accepted_new in the order they were sampled
            num_simulated += prmtr_sampled.size();
            check_parameters(prmtr_sampled, prmtr_accepted_new);
            num_simulated -= prmtr_sampled.size();

            // Compute weights for accepted parameters
            compute_weights(smc_sampler, input_obj.perturbation_pdf,
                            t, prmtr_accepted_new, weights_new);

            // If terminate_program is set, return
            if (terminate_program) return;

            // If we have enough parameters in this generation,
            // break out of control loop
            if (prmtr_accepted_new.size() >= pop_size)
                break;

            // Sleep for MAIN_TIMEOUT
            std::this_thread::sleep_for(MAIN_TIMEOUT);
        } // control loop

        // Pop any superfluous parameters & weights
        if (prmtr_accepted_new.size() != weights_new.size()) {
            std::runtime_error e("prmtr_accepted_new and weights_new should have same size");
            throw e;
        }
        while (prmtr_accepted_new.size() > pop_size) {
            prmtr_accepted_new.pop_back();
            weights_new.pop_back();
            num_simulated--;
        }

        // Swap population and weights
        smc_sampler.swap_population(weights_new, prmtr_accepted_new);
        weights_new.resize(0);
        prmtr_accepted_new.resize(0);

        // Print time and number of simulations
        stop_timer();
        std::cerr << "Completed in " << elapsed_time() << " seconds\n";
        fprintf(stderr, "Accepted/simulated: %d/%d (%5.2f%%)\n", pop_size, num_simulated,
                (100.0 * pop_size / (double) num_simulated));

        if (t < input_obj.epsilons.size() - 1) {
            // Send signal to managers to signal next generation
            send_signal_to_idle_managers(TERMINATE_PROCESS_SIGNAL, idle_managers);

            // Check result messages until you have received results from all
            // managers. Since all managers send either a valid result,
            // or a cancellation result, all managers are guaranteed
            // to have no running simulations anymore after this point
            while (idle_managers.size() < size) {
                check_managers(manager_map, idle_managers);
                // Sleep for MAIN_TIMEOUT
                std::this_thread::sleep_for(MAIN_TIMEOUT);
            }
        }
    } // main loop

    // Terminate all managers
    send_signal_to_managers(TERMINATE_MANAGER_SIGNAL);

    // Output accepted parameter values as comma-separated list
    write_parameters(std::cout, input_obj.parameter_names,
            smc_sampler.getParameterPopulation());
}

}

namespace sweep {

void master(const input_t& input_obj) {

    // Get size
    const int size = MPI::COMM_WORLD.Get_size();

    // Create generator object
    Generator generator_obj(input_obj.generator);

    // Total number of parameters
    const int num_param = generator_obj.getNumberOfParameters();

    // Initialize idle managers set
    std::set<int> idle_managers;
    for (int i = 0; i < size; i++)
        idle_managers.insert(i);

    // Initialize queue to hold sampled parameter handlers
    std::queue<ParameterHandler> prmtr_sampled;

    // Initialize manager to parameter handler map
    std::vector<ParameterHandler*> manager_map(size);

    // Initialize prmtr_accepted
    std::vector<parameter_t> prmtr_accepted;

    // Print message
    std::cerr << "Computing " << num_param << " parameters\n";
    start_timer();

    // Start main loop
    for (;;) {

        // Check if any managers have finished their work
        check_managers(manager_map, idle_managers);

        // Send work to idle managers
        delegate_managers(generator_obj, "0", manager_map, idle_managers,
                          prmtr_sampled);

        // Add parameter to prmtr_accepted in the order they were generated
        int num_prmtr_accepted_old = prmtr_accepted.size();
        check_parameters(prmtr_sampled, prmtr_accepted);

#ifndef NDEBUG
        if (num_prmtr_accepted_old != prmtr_accepted.size())
            std::cerr << "prmtr_accepted.size(): " << prmtr_accepted.size() <<
                std::endl;
#endif
        // If we have enough parameters
        if (prmtr_accepted.size() >= num_param) {

            // Terminate all managers
            send_signal_to_managers(TERMINATE_MANAGER_SIGNAL);

#ifndef NDEBUG
            // Print if there are any superfluous parameters
            if (prmtr_accepted.size() > num_param)
                std::cerr << "There were " << (prmtr_accepted.size() - num_param)
                          << " too many parameters generated\n";
#endif

            // Pop any superfluous parameters
            while (prmtr_accepted.size() > num_param)
                prmtr_accepted.pop_back();

            // Print time and number of simulations
            stop_timer();
            std::cerr << "Completed in " << elapsed_time() << " seconds\n";

            // Output accepted parameter values as comma-separated list
            write_parameters(std::cout, input_obj.parameter_names, prmtr_accepted);

            // Return
            return;
        }

        // If terminate_program is set, return
        if (terminate_program) return;

        // Sleep for MAIN_TIMEOUT
        std::this_thread::sleep_for(MAIN_TIMEOUT);
    }
}

}
