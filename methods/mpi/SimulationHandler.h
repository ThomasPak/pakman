#ifndef SIMULATIONHANDLER_H
#define SIMULATIONHANDLER_H

#include <string>
#include <thread>
#include <atomic>

#include <mpi.h>

#include "../pipe_io.h"
#include "../types.h"

class AbstractSimulationHandler {

    public:

        // Construct from epsilon, simulator and parameter sample
        AbstractSimulationHandler(const std::string& epsilon, const cmd_t& simulator,
                                  const parameter_t& prmtr_sample);

        // Virtual destructor
        virtual ~AbstractSimulationHandler() = default;

        // Virtual function getResult() to probe result of simulation
        virtual const std::string* getResult() = 0;

        // Virtual function terminate() to prematurely terminate simulation
        virtual void terminate() = 0;

    protected:

        // Epsilon
        const std::string m_epsilon;

        // Simulator command
        const cmd_t m_simulator;

        // Parameter sample
        const parameter_t m_prmtr_sample;

        // Read buffer
        std::string m_read_buffer;
};

class SimulationHandler : public AbstractSimulationHandler {

    public:

        // Construct from epsilon, simulator and parameter sample
        SimulationHandler(const std::string& epsilon, const cmd_t& simulator,
                          const parameter_t& prmtr_sample);

        // Destructor
        virtual ~SimulationHandler() override;

        // Terminate simulation
        virtual void terminate() override;

        // Get result, of simulation. If result is not ready, return nullptr
        virtual const std::string* getResult() override;

    private:

        // Process id of simulator
        pid_t m_child_pid;

        // File descriptors for pipes
        int m_pipe_write_fd;
        int m_pipe_read_fd;

        // Read pipe status flag
        bool m_read_done = false;
};

class MPISimulationHandler : public AbstractSimulationHandler {

    public:

        // Construct from epsilon, simulator and parameter sample
        MPISimulationHandler(const std::string& epsilon, const cmd_t& simulator,
                          const parameter_t& prmtr_sample);

        // Destructor
        virtual ~MPISimulationHandler() override;

        // Terminate simulation
        virtual void terminate() override;

        // Get result, of simulation. If result is not ready, return nullptr
        virtual const std::string* getResult() override;

    private:

        // Intercomm with child
        MPI::Intercomm m_child_comm;

        // Flag for receiving result
        bool m_result_received = false;
};

#endif // SIMULATIONHANDLER_H
