#include <iostream>
#include <string>
#include <stdexcept>
#include <atomic>

#include <mpi.h>
#include <signal.h>

#include "../types.h"
#include "../run_simulation.h"
#include "../read_input.h"
#include "mpi_helper.h"
#include "common.h"
#include "SimulationHandler.h"
#include "manager.h"

// Flags
bool terminate_simulation = false;
bool terminate_manager = false;
std::atomic<bool> terminate_program(false);

// Epsilon iterators
std::vector<std::string>::const_iterator epsilon;
std::vector<std::string>::const_iterator epsilon_end;

void set_terminate_flag(int signal) {
    switch (signal) {
        case SIGINT:
        case SIGTERM:
            terminate_program = true;
            break;
    }
}

void set_signal_handler() {

    struct sigaction act;

    act.sa_handler = set_terminate_flag;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, &act, nullptr);
    sigaction(SIGTERM, &act, nullptr);
}

void receive_raw_input(std::string& raw_input) {

    // Receive raw input
    char *buffer;
    Dynamic_Bcast(MPI::COMM_WORLD, buffer, 0, MPI::CHAR, MASTER);
    raw_input.assign(buffer);
    delete[] buffer;
}

void process_signal(const MPI::Status& status) {

    int count = status.Get_count(MPI::INT), tag = status.Get_tag(), source = status.Get_source();

    if (tag != SIGNAL_TAG) {
        std::runtime_error e("tag should be SIGNAL_TAG");
        throw e;
    }

    if (count != 1) {
        std::string error_msg("signal should be single integer");
        error_msg += ", count: ";
        error_msg += std::to_string(count);
        std::runtime_error e(error_msg);
        throw e;
    }

    if (source != MASTER) {
        std::runtime_error e("signal should come from master");
        throw e;
    }

    // Receive signal
    int signal;
    MPI::COMM_WORLD.Recv(&signal, count, MPI::INT, source, tag);

    // Process signal
    switch (signal) {
        case TERMINATE_MANAGER_SIGNAL:
            terminate_manager = true;
            break;
        case NEXT_GENERATION_SIGNAL:
            // Set flag to terminate current simulation
            terminate_simulation = true;

            // Increment epsilon iterator
            epsilon++;
            if (epsilon >= epsilon_end) {
                std::runtime_error e("not enough epsilon values");
                throw e;
            }
            break;
        default:
            std::runtime_error e("did not recognize signal");
            throw e;
    }
}

void process_parameter(const MPI::Status& status,
                       const std::string& epsilon,
                       const cmd_t& simulator,
                       AbstractSimulationHandler*& sim_handler) {

    // sim_handler should be nullptr
    if (sim_handler) {
        std::runtime_error e("simulation is already running, "
                             "manager should not be receiving parameters");
        throw e;
    }

    int count = status.Get_count(MPI::CHAR), tag = status.Get_tag(), source = status.Get_source();

    if (tag != PARAMETER_TAG) {
        std::runtime_error e("tag should be PARAMETER_TAG");
        throw e;
    }

    if (source != MASTER) {
        std::runtime_error e("parameter should come from master");
        throw e;
    }

    // Get parameter
    char *buffer = new char[count];
    MPI::COMM_WORLD.Recv(buffer, count, MPI::CHAR, source, tag);
    parameter_t prmtr_received(buffer);
    delete[] buffer;

    // Start simulation
    if (mpi_simulator)
        sim_handler = new MPISimulationHandler(epsilon, simulator, prmtr_received);
    else
        sim_handler = new SimulationHandler(epsilon, simulator, prmtr_received);
}

void check_simulation(AbstractSimulationHandler*& sim_handler) {

    if (sim_handler == nullptr) {
        std::runtime_error e("simulation is not running");
        throw e;
    }

    const std::string *result_str = sim_handler->getResult();

    if (result_str) { // Simulation is finished and result is ready

        // Send result to master
        int result = simulation_result(*result_str);
        MPI::COMM_WORLD.Send(&result, 1, MPI::INT, MASTER, RESULT_TAG);

        // Clean up simulation handler
        delete sim_handler; sim_handler = nullptr;
   }
}

void manager(const std::vector<std::string>& epsilons,
             const cmd_t& simulator) {

    // Set signal handler
    set_signal_handler();

    // Initialize SimulationHandler pointer
    AbstractSimulationHandler *sim_handler = nullptr;

    // Initialize epsilon iterators
    epsilon = epsilons.cbegin();
    epsilon_end = epsilons.cend();

    // Loop
    for (;;) {

        // If simulation is running, check up on it
        if (sim_handler) check_simulation(sim_handler);

        // Probe for message
        MPI::Status status;
        if ( MPI::COMM_WORLD.Iprobe(MASTER, SIGNAL_TAG, status)
             || MPI::COMM_WORLD.Iprobe(MASTER, PARAMETER_TAG, status) ) {

            // Switch based on tag
            int tag = status.Get_tag();

#ifndef NDEBUG
            const int rank = MPI::COMM_WORLD.Get_rank();
            const int size = MPI::COMM_WORLD.Get_size();
            switch (tag) {
                case SIGNAL_TAG:
                    std::cerr << "Manager " << rank << "/" << size <<
                    ": received signal message from master" << std::endl;
                    break;
                case PARAMETER_TAG:
                    std::cerr << "Manager " << rank << "/" << size <<
                    ": received parameter message from master" << std::endl;
                    break;
            }
#endif

            switch (tag) {
                case SIGNAL_TAG:
                    process_signal(status);
                    break;
                case PARAMETER_TAG:
                    process_parameter(status, *epsilon, simulator, sim_handler);
                    break;
                default:
                    std::runtime_error e("did not recognize message tag");
                    throw e;
            }
        } // If a message was received

        // If terminate_simulation, terminate_manager or terminate_program is set,
        // terminate simulation and reset terminate_simulation flag
        // Also send cancellation message to master
        if (terminate_simulation || terminate_manager || terminate_program) {
            if (sim_handler) {
                sim_handler->terminate();
                delete sim_handler; sim_handler = nullptr;

                if (terminate_simulation) {
                    int cancel_result = CANCEL;
                    MPI::COMM_WORLD.Send(&cancel_result, 1, MPI::INT, MASTER, RESULT_TAG);
                }
            }
            terminate_simulation = false;

            // If terminate_manager or terminate_program is set, return
            if (terminate_manager || terminate_program) return;
        }

        // Sleep for MAIN_TIMEOUT
        std::this_thread::sleep_for(MAIN_TIMEOUT);
    }
}
