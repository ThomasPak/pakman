#include <iostream>
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

#include "../types.h"
#include "../read_input.h"
#include "../system_call.h"
#include "../write_parameters.h"
#include "../run_simulation.h"
#include "../Sampler.h"
#include "../smc_weight.h"
#include "mpi_utils.h"
#include "common.h"
#include "Master.h"
#include "../timer.h"

// Do normal stuff
void AbstractMaster::doNormalStuff()
{
    // This function should never be called if the Master has
    // terminated
    assert(m_state != terminated);

    // Check for program termination interrupt
    if (*m_p_program_terminated)
    {
        // Terminate Master
        m_state = terminated;
        return;
    }

    // Check for termination of Master and Managers
    if (m_master_manager_terminated)
    {
        // Send TERMINATE_MANAGER_SIGNAL to all Managers
        sendSignalToAllManagers(TERMINATE_MANAGER_SIGNAL);

        // Terminate Master
        m_state = terminated;
        return;
    }

    // Check for termination of Workers
    if (m_worker_terminated)
    {
        // Send TERMINATE_PROCESS_SIGNAL to all Managers
        sendSignalToAllManagers(TERMINATE_PROCESS_SIGNAL);

        // Flush all TaskHandler queues
        flushQueues();

        // Reset flag
        m_worker_terminated = false;

        // Switch to flushing state
        m_state = flushing;
        return;
    }

    // Listen to Managers
    listenToManagers();

    // Pop finished tasks from busy queue and insert into finished queue
    popBusyQueue();

    // Queue new tasks and extract finished tasks
    processTasks();

    // Delegate tasks to Managers
    delegateToManagers();
}

// Do flushing stuff
void AbstractMaster::doFlushingStuff()
{
    // This function should never be called if the Master has
    // terminated
    assert(m_state != terminated);

    // Check for program termination interrupt
    if (*m_p_program_terminated)
    {
        // Terminate Master
        m_state = terminated;
        return;
    }

    // Check for termination of Master and Managers
    if (m_master_manager_terminated)
    {
        // Send TERMINATE_MANAGER_SIGNAL to all Managers
        sendSignalToAllManagers(TERMINATE_MANAGER_SIGNAL);

        // Terminate Master
        m_state = terminated;
        return;
    }

    // Discard message and signals
    discardMessagesAndSignals();

    // If all Managers are idle, transition to normal state
    if (m_idle_managers.size() == m_comm_size)
    {
        m_state = normal;
        return;
    }
}

// Queue task
void AbstractMaster::queueTask(const std::string& input_string)
{
    m_pending_tasks.push(input_string);
}

// Pop finished tasks
void AbstractMaster::popFinishedTasks(
        std::vector<AbstractMaster::TaskHandler>& popped_tasks)
{
    // Move m_finished_tasks to popped_tasks to avoid copying
    popped_tasks = std::move(m_finished_tasks);

    // Clear m_finished_tasks
    m_finished_tasks.clear();
}

// Listen to messages from Managers.
void AbstractMaster::listenToManagers()
{
    // While there are any incoming messages
    while (probeMessage())
    {
        // Probe manager
        int manager_rank = probeMessageManager();

        // Receive message
        std::string&& output_string = receiveMessage(manager_rank);

        // Record output string
        m_map_manager_to_task[manager_rank]->recordOutput(output_string);

        // Mark manager as idle
        m_idle_managers.insert(manager_rank);
    }
}

void AbstractMaster::popBusyQueue()
{
    // If busy queue is empty, return immediately
    if (m_busy_tasks.empty())
        return;

    // While there are finished tasks (or tasks where errors occured) in the
    // front of the queue
    while (!m_busy_tasks.empty() && !m_busy_tasks.front().isPending())
    {
#ifndef NDEBUG
        std::cerr << "DEBUG: AbstractMaster::popBusyQueue: ";
        std::cerr << "Moving TaskHandler from busy to finished!\n";
        std::cerr << "DEBUG: finished, busy, pending: "
            << m_finished_tasks.size() << ", " << m_busy_tasks.size()
            << ", " << m_pending_tasks.size() << std::endl;
#endif
        // Move TaskHandler to finished tasks
        m_finished_tasks.push_back(std::move(m_busy_tasks.front()));

        // Pop front TaskHandler from busy queue
        m_busy_tasks.pop();
#ifndef NDEBUG
        std::cerr << "DEBUG: AbstractMaster::popBusyQueue: ";
        std::cerr << "Done moving TaskHandler from busy to finished!\n";
        std::cerr << "DEBUG: finished, busy, pending: "
            << m_finished_tasks.size() << ", " << m_busy_tasks.size()
            << ", " << m_pending_tasks.size() << std::endl;
#endif
    }
}

// Delegate to Managers
void AbstractMaster::delegateToManagers()
{
    // While there are idle managers
    auto it = m_idle_managers.begin();
    for (; (it != m_idle_managers.end()) && !m_pending_tasks.empty(); it++)
    {
#ifndef NDEBUG
        std::cerr << "DEBUG: AbstractMaster::delegateToManagers: ";
        std::cerr << "Moving TaskHandler from pending to busy!\n";
        std::cerr << "DEBUG: finished, busy, pending: "
            << m_finished_tasks.size() << ", " << m_busy_tasks.size()
            << ", " << m_pending_tasks.size() << std::endl;
#endif
        // Send message to Manager
        sendMessageToManager(*it, m_pending_tasks.front().getInputString());

        // Move pending TaskHandler to busy queue
        m_busy_tasks.push(std::move(m_pending_tasks.front()));

        // Pop front TaskHandler from pending queue
        m_pending_tasks.pop();

        // Set map from Manager to TaskHandler
        m_map_manager_to_task[*it] = &m_busy_tasks.back();
#ifndef NDEBUG
        std::cerr << "DEBUG: AbstractMaster::delegateToManagers: ";
        std::cerr << "Done moving TaskHandler from pending to busy!\n";
        std::cerr << "DEBUG: finished, busy, pending: "
            << m_finished_tasks.size() << ", " << m_busy_tasks.size()
            << ", " << m_pending_tasks.size() << std::endl;
#endif
    }

    // Mark Managers as busy
    m_idle_managers.erase(m_idle_managers.begin(), it);
}

// Flush all task queues (finished, busy, pending)
void AbstractMaster::flushQueues()
{
    m_finished_tasks.clear();
    while (!m_busy_tasks.empty()) m_busy_tasks.pop();
    while (!m_pending_tasks.empty()) m_pending_tasks.pop();
}

// Discard any messages and signals until all Managers are idle
void AbstractMaster::discardMessagesAndSignals()
{
    // While there are any incoming messages
    while (probeMessage())
    {
        // Probe manager
        int manager_rank = probeMessageManager();

        // Receive and discard message
        receiveMessage(manager_rank);

        // Mark manager as idle
        m_idle_managers.insert(manager_rank);
    }

    // While there are any incoming signals
    while (probeSignal())
    {
        // Probe manager
        int manager_rank = probeSignalManager();

        // If it a cancellation signal, mark manager as idle
        if (receiveSignal(manager_rank) == PROCESS_CANCELLED_SIGNAL)
            m_idle_managers.insert(manager_rank);
    }
}

// Probe for message
bool AbstractMaster::probeMessage() const
{
    return MPI::COMM_WORLD.Iprobe(MPI_ANY_SOURCE, OUTPUT_TAG);
}

// Probe for signal
bool AbstractMaster::probeSignal() const
{
    MPI::COMM_WORLD.Iprobe(MPI_ANY_SOURCE, MANAGER_SIGNAL_TAG);
}

// Probe for Manager rank of incoming message
int AbstractMaster::probeMessageManager() const
{
    MPI::Status status;
    MPI::COMM_WORLD.Probe(MPI_ANY_SOURCE, OUTPUT_TAG, status);
    return status.Get_source();
}

// Probe for Manager rank of incoming signal
int AbstractMaster::probeSignalManager() const
{
    MPI::Status status;
    MPI::COMM_WORLD.Probe(MPI_ANY_SOURCE, MANAGER_SIGNAL_TAG, status);
    return status.Get_source();
}

// Receive message from Manager
std::string AbstractMaster::receiveMessage(int manager_rank) const
{
    // Sanity check: probeMessage must return true
    assert(probeMessage());

    // Probe message to get status
    MPI::Status status;
    MPI::COMM_WORLD.Probe(manager_rank, OUTPUT_TAG, status);

    // Sanity check on message
    assert(status.Get_tag() == OUTPUT_TAG);
    assert(status.Get_source() == manager_rank);

    // Receive message from Manager
    int count = status.Get_count(MPI::CHAR);
    char *buffer = new char[count];
    MPI::COMM_WORLD.Recv(buffer, count, MPI::CHAR, manager_rank, OUTPUT_TAG);

    // Return input as string
    std::string input_string(buffer);
    delete[] buffer;
    return input_string;
}

// Receive signal from Manager
int AbstractMaster::receiveSignal(int manager_rank) const
{
    // Sanity check: probeSignal must return true
    assert(probeSignal());

    // Probe signal message to get status
    MPI::Status status;
    MPI::COMM_WORLD.Probe(manager_rank, MANAGER_SIGNAL_TAG, status);

    // Sanity check on signal, which has to be a single integer
    assert(status.Get_tag() == MANAGER_SIGNAL_TAG);
    assert(status.Get_source() == manager_rank);
    assert(status.Get_count(MPI::INT) == 1);

    // Receive signal from Manager
    int signal;
    MPI::COMM_WORLD.Recv(&signal, 1, MPI::INT, manager_rank,
            MANAGER_SIGNAL_TAG);

    // Return signal as integer
    return signal;
}

// Send message to a Manager
void AbstractMaster::sendMessageToManager(int manager_rank,
        const std::string& message_string)
{
#ifndef NDEBUG
    std::cerr << "AbstractMaster::sendMessageToManager: sending to manager_rank " << manager_rank
        << " and message:\n" << message_string << std::endl;
    std::cerr << "Idle managers:\n";
    for (auto it = m_idle_managers.begin(); it != m_idle_managers.end(); it++)
        std::cerr << *it << std::endl;
    std::cerr << "-- END --\n";
#endif
    // Ensure previous message has finished sending
    m_message_requests[manager_rank].Wait();

    // Store message string in buffer
    m_message_buffers[manager_rank].assign(message_string);

    // Note: Isend is used here to avoid deadlock since the Master and the root
    // Manager are executed by the same process
    m_message_requests[manager_rank] = MPI::COMM_WORLD.Isend(
            m_message_buffers[manager_rank].c_str(),
            m_message_buffers[manager_rank].size() + 1,
            MPI::CHAR, manager_rank, INPUT_TAG);
}

// Send signal to all Managers
void AbstractMaster::sendSignalToAllManagers(int signal)
{
    // Ensure previous signals have finished sending
    for (int manager_rank = 0; manager_rank < m_comm_size; manager_rank++)
        m_signal_requests[manager_rank].Wait();

    // Store signal in buffer
    m_signal_buffer = signal;

    // Note: Isend is used here to avoid deadlock since the Master and the root
    // Manager are executed by the same process
    for (int manager_rank = 0; manager_rank < m_comm_size; manager_rank++)
        m_signal_requests[manager_rank] =
            MPI::COMM_WORLD.Isend(&m_signal_buffer, 1, MPI::INT, manager_rank,
                    MASTER_SIGNAL_TAG);
}

void ABCRejectionMaster::processTasks()
{
    // Check if there are any new accepted parameters
    if (numberOfFinishedTasks() > 0)
    {
        // Get finished tasks
        std::vector<TaskHandler> finished_tasks;
        popFinishedTasks(finished_tasks);

        // Move finished tasks to m_prmtr_accepted
        for (auto it = finished_tasks.begin();
                it != finished_tasks.end(); it++)
        {
            // Do not accept any errors for now
            assert(!it->didErrorOccur());

            // Check if parameter was accepted
            if (simulation_result(it->getOutputString()) == ACCEPT)
            {
                // Get string
                std::string input_string = it->getInputString();

                // Trim epsilon
                input_string.erase(0, 2);

                // Push accepted parameter
                m_prmtr_accepted.push_back(input_string);
            }

#ifndef NDEBUG
            std::cerr << "it->getInputString(): " << it->getInputString() <<
                std::endl;
#endif
        }
    }

    // If enough parameters have been accepted, print them and terminate Master
    // and Managers.
    if (m_prmtr_accepted.size() >= m_num_accept)
    {
        // Trim any superfluous parameters
        while (m_prmtr_accepted.size() > m_num_accept)
            m_prmtr_accepted.pop_back();

        // Print accepted parameters
        write_parameters(std::cout, m_parameter_names, m_prmtr_accepted);

        // Terminate Master and Managers and return
        terminateMasterAndManagers();
        return;
    }

    // There is still work to be done, so make sure there are as many tasks
    // queued as there are Managers
    while (numberOfPendingTasks() < m_comm_size)
        queueTask(simulator_input(m_epsilon,
                    m_prior_sampler.sampleParameter()));
}

void check_managers(const std::vector<ParameterHandler*>& manager_map,
                    std::set<int>& idle_managers) {

    // While there are any incoming output messages
    MPI::Status status;
    while (MPI::COMM_WORLD.Iprobe(MPI_ANY_SOURCE, OUTPUT_TAG, status)) {

        // Receive output message
        int count = status.Get_count(MPI::CHAR);
        int manager = status.Get_source();
        char *buffer = new char[count];
        MPI::COMM_WORLD.Recv(buffer, count, MPI::CHAR, manager, OUTPUT_TAG);

#ifndef NDEBUG
        const int rank = MPI::COMM_WORLD.Get_rank();
        const int size = MPI::COMM_WORLD.Get_size();
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
    while (MPI::COMM_WORLD.Iprobe(MPI_ANY_SOURCE, MANAGER_SIGNAL_TAG, status)) {

        // Receive Manager signal
        int signal, manager = status.Get_source();
        MPI::COMM_WORLD.Recv(&signal, 1, MPI::INT, manager,
                MANAGER_SIGNAL_TAG);

#ifndef NDEBUG
        const int rank = MPI::COMM_WORLD.Get_rank();
        const int size = MPI::COMM_WORLD.Get_size();
        std::cerr << "Master " << rank << "/" << size
                  << ": received signal from manager "
                  << manager << std::endl;
#endif

        // Set parameter status appropriately according to signal received
        switch (signal) {
            case PROCESS_CANCELLED_SIGNAL:
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
        reqs.push_back(MPI::COMM_WORLD.Isend(&signal, 1, MPI::INT, manager_idx, MASTER_SIGNAL_TAG));

    for (auto& req : reqs) req.Wait();
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

            // If program_terminated is set, return
            if (program_terminated) return;

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
            send_signal_to_managers(TERMINATE_PROCESS_SIGNAL);

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

        // If program_terminated is set, return
        if (program_terminated) return;

        // Sleep for MAIN_TIMEOUT
        std::this_thread::sleep_for(MAIN_TIMEOUT);
    }
}

}
