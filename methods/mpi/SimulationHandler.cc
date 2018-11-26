#include <string>
#include <thread>
#include <atomic>
#include <stdexcept>

#include <unistd.h>
#include <signal.h>
#include <sys/utsname.h>

#include "../types.h"
#include "../system_call.h"
#include "../pipe_io.h"
#include "../run_simulation.h"

#include "common.h"
#include "spawn.h"
#include "SimulationHandler.h"

#ifndef NDEBUG
#include <iostream>
#endif

AbstractSimulationHandler::AbstractSimulationHandler(
        const std::string& epsilon,
        const cmd_t& simulator,
        const parameter_t& prmtr_sample) :
    m_epsilon(epsilon),
    m_simulator(simulator),
    m_prmtr_sample(prmtr_sample) {}

SimulationHandler::SimulationHandler(const std::string& epsilon, const cmd_t& simulator,
                                     const parameter_t& prmtr_sample) :
    AbstractSimulationHandler(epsilon, simulator, prmtr_sample) {

    // Start simulation
    system_call(m_simulator, m_child_pid, m_pipe_write_fd, m_pipe_read_fd);

    // Write input to simulator
    std::string write_buffer;
    simulator_input(m_epsilon, m_prmtr_sample, write_buffer);
#ifndef NDEBUG
    std::cerr << "write_buffer: " << write_buffer << std::endl;
#endif
    write_to_pipe(m_pipe_write_fd, write_buffer);
    close_check(m_pipe_write_fd);
}

SimulationHandler::~SimulationHandler() {

#ifndef NDEBUG
    std::cerr << "Destroying...\n";
#endif
    // Wait on child process if it has not yet been waited for
    if (m_child_pid) waitpid_success(m_child_pid, 0, m_simulator);

    // Close pipe if not already closed
    if (!m_read_done) close_check(m_pipe_read_fd);
}

void SimulationHandler::terminate() {

#ifndef NDEBUG
    std::cerr << "Terminating...\n";
#endif

#ifndef NDEBUG
    std::cerr << "Checking whether simulation has finished...\n";
#endif
    // If simulation has finished, mark by setting m_child_pid to zero
    if ( waitpid_success(m_child_pid, WNOHANG, m_simulator) ) {
        m_child_pid = 0;
        return;
    }

#ifndef NDEBUG
    std::cerr << "Sending sigterm to child with pid " << m_child_pid << "...\n";
#endif
    // Send SIGTERM to child process
    if ( kill(m_child_pid, SIGTERM) ) {
        std::runtime_error e("an error occurred while trying to terminate "
                             "child process");
        throw e;
    }

#ifndef NDEBUG
    std::cerr << "Sleeping before taking more drastic measures...\n";
#endif
    // Sleep for KILL_TIMEOUT
    std::this_thread::sleep_for(KILL_TIMEOUT);

#ifndef NDEBUG
    std::cerr << "Checking whether sigterm worked...\n";
#endif
    // If simulation has finished, mark by setting m_child_pid to zero
    if ( waitpid_success(m_child_pid, WNOHANG, m_simulator, ignore_error) ) {
        m_child_pid = 0;
        return;
    }

#ifndef NDEBUG
    std::cerr << "Sending sigkill to child...\n";
#endif
    // Send SIGKILL to child process
    if ( kill(m_child_pid, SIGKILL) ) {
        std::runtime_error e("an error occurred while trying to kill "
                             "child process");
        throw e;
    }

#ifndef NDEBUG
    std::cerr << "Waiting on child...\n";
#endif
     waitpid_success(m_child_pid, 0, m_simulator, ignore_error);
     m_child_pid = 0;
}

const std::string* SimulationHandler::getResult() {

    // Poll pipe. If pipe is finished reading, close pipe and
    // return pointer to m_read_buffer, else return nullptr
    if ( poll_read_from_pipe(m_pipe_read_fd, m_read_buffer) ) {
        close_check(m_pipe_read_fd);
        m_read_done = true;
        return &m_read_buffer;
    }
    else
        return nullptr;
}

MPISimulationHandler::MPISimulationHandler(const std::string& epsilon, const cmd_t& simulator,
                                     const parameter_t& prmtr_sample) :
    AbstractSimulationHandler(epsilon, simulator, prmtr_sample) {
#ifndef NDEBUG
    const int rank = MPI::COMM_WORLD.Get_rank();
    const int size = MPI::COMM_WORLD.Get_size();
    std::cerr << "Manager " << rank << "/" << size << ": MPI simulation constructing...\n";
#endif

    // Create MPI::Info object
    MPI::Info info = MPI::Info::Create();

    // Ensure process is spawned on same node if force_host_spawn is set
    if (force_host_spawn) {
        struct utsname buf;
        uname(&buf);
        info.Set("host", buf.nodename);
    }

    m_child_comm = spawn(simulator, info);

    // Free MPI::Info object
    info.Free();

    // Write input to simulator
    std::string write_buffer;
    simulator_input(m_epsilon, m_prmtr_sample, write_buffer);
    m_child_comm.Send(write_buffer.c_str(), write_buffer.size() + 1, MPI::CHAR, 0, 0);
}

MPISimulationHandler::~MPISimulationHandler() {
#ifndef NDEBUG
    const int rank = MPI::COMM_WORLD.Get_rank();
    const int size = MPI::COMM_WORLD.Get_size();
    std::cerr << "Manager " << rank << "/" << size << ": MPI simulation destroying...\n";
#endif
    terminate();

    // Free communicator
    m_child_comm.Disconnect();
}

void MPISimulationHandler::terminate() {

#ifndef NDEBUG
    const int rank = MPI::COMM_WORLD.Get_rank();
    const int size = MPI::COMM_WORLD.Get_size();
    std::cerr << "Manager " << rank << "/" << size << ": MPI simulation terminating...\n";
#endif
    // MPI does not provide process control, so
    // we can only wait for the simulation to finish
    // if it has not finished yet
    if (!m_result_received) {
        // Source and tag are both zero
        const int source = 0, tag = 0;

        MPI::Status status;
        while (!m_child_comm.Iprobe(0, 0, status))
            // Sleep for MAIN_TIMEOUT
            std::this_thread::sleep_for(MAIN_TIMEOUT);

        const int count = status.Get_count(MPI::CHAR);
        char *buffer = new char[count];

        m_child_comm.Recv(buffer, count, MPI::CHAR, source, tag);

        m_result_received = true;

        delete[] buffer;
    }
}

const std::string* MPISimulationHandler::getResult() {

    // Probe for result.
    MPI::Status status;
    if (m_child_comm.Iprobe(0, 0, status)) {
        // Source and tag are both zero
        const int source = 0, tag = 0;

        const int count = status.Get_count(MPI::CHAR);
        char *buffer = new char[count];

        m_child_comm.Recv(buffer, count, MPI::CHAR, source, tag);

        m_read_buffer.assign(buffer);
        delete[] buffer;

        m_result_received = true;

        return &m_read_buffer;
    } else return nullptr;
}
