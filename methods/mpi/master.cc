#include <iostream>
#include <memory>
#include <cassert>
#include <string>
#include <queue>
#include <vector>
#include <set>
#include <thread>
#include <random>
#include <chrono>
#include <stdexcept>
#include <mpi.h>

#include "types.h"
#include "read_input.h"
#include "system_call.h"
#include "write_parameters.h"
#include "run_simulation.h"
#include "Sampler.h"
#include "smc_weight.h"
#include "timer.h"
#include "common.h"
#include "mpi_utils.h"
#include "mpi_common.h"
#include "ParameterHandler.h"

void check_managers(const std::vector<ParameterHandler*>& manager_map,
                    std::set<int>& idle_managers) {

    // While there are any incoming output messages
    MPI_Status status;
    while (iprobe_wrapper(MPI_ANY_SOURCE, MANAGER_MSG_TAG,
                MPI_COMM_WORLD, &status))
    {

        // Receive output message
        int count = 0, manager = 0;
        MPI_Get_count(&status, MPI_CHAR, &count);
        manager = status.MPI_SOURCE;
        char *buffer = new char[count];
        MPI_Recv(buffer, count, MPI_CHAR, manager, MANAGER_MSG_TAG,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);

#ifndef NDEBUG
        const int rank = get_mpi_comm_world_rank();
        const int size = get_mpi_comm_world_size();
        std::cerr << "Master " << rank << "/" << size
                  << ": received result message from manager "
                  << manager << std::endl;
#endif

        // Interpret output message
        int result = simulation_result(buffer);
        delete[] buffer;

        // Set parameter status appropriately according to output message
        // received
        switch (result) {
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

            default:
                throw;
        }

        // Mark manager as idle
        idle_managers.insert(manager);
    }

    // Check for Manager signals
    while (iprobe_wrapper(MPI_ANY_SOURCE, MANAGER_SIGNAL_TAG,
                MPI_COMM_WORLD, &status)) {

        // Receive Manager signal
        int signal, manager = status.MPI_SOURCE;
        MPI_Recv(&signal, 1, MPI_INT, manager, MANAGER_SIGNAL_TAG,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);

#ifndef NDEBUG
        const int rank = get_mpi_comm_world_rank();
        const int size = get_mpi_comm_world_size();
        std::cerr << "Master " << rank << "/" << size
                  << ": received signal from manager "
                  << manager << std::endl;
#endif

        // Set parameter status appropriately according to signal received
        switch (signal) {
            case WORKER_FLUSHED_SIGNAL:
                manager_map[manager]->setStatus(cancelled);
                idle_managers.insert(manager);
                break;

            default:
                throw;
        }
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
        MPI_Request dummy_request;
        MPI_Isend(input_str.c_str(), input_str.size() + 1, MPI_CHAR, *it,
                MASTER_MSG_TAG, MPI_COMM_WORLD, &dummy_request);
        MPI_Request_free(&dummy_request);

#ifndef NDEBUG
        const int rank = get_mpi_comm_world_rank();
        const int size = get_mpi_comm_world_size();
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

    const int size = get_mpi_comm_world_size();

    std::vector<MPI_Request> reqs;

    for (int manager_idx = 0; manager_idx < size; manager_idx++)
    {
        MPI_Request request;
        MPI_Isend(&signal, 1, MPI_INT, manager_idx, MASTER_SIGNAL_TAG,
                MPI_COMM_WORLD, &request);
        reqs.push_back(request);
    }

    for (auto& req : reqs)
        MPI_Wait(&req, MPI_STATUS_IGNORE);
}

namespace mcmc {

void master() {

    // Get rank and size
    const int size = get_mpi_comm_world_size();
    const int rank = get_mpi_comm_world_rank();

    std::cerr << "Master: rank " << rank << "/" << size << std::endl;
}

}

namespace sweep {

void master(const input_t& input_obj) {

    // Get size
    const int size = get_mpi_comm_world_size();

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

        // If program_terminated is set, return
        if (program_terminated) return;

        // Sleep for MAIN_TIMEOUT
        std::this_thread::sleep_for(MAIN_TIMEOUT);
    }
}

}
