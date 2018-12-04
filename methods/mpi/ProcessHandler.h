#ifndef PROCESSHANDLER_H
#define PROCESSHANDLER_H

#include <string>
#include <thread>

#include <mpi.h>

#include "../pipe_io.h"
#include "../types.h"

class AbstractProcessHandler
{

    public:

        // Construct from command string and input string
        AbstractProcessHandler(const cmd_t& command, const std::string& input_string);

        // Virtual destructor
        virtual ~AbstractProcessHandler() = default;

        // Virtual function terminate() to prematurely terminate process
        virtual void terminate() = 0;

        // Virtual function isDone() to check whether process has finished
        virtual bool isDone() = 0;

        // Function getOutput() to get output of running command
        // Running this command before process is finished will result in
        // an error, so always check with isDone() first.
        std::string getOutput();

    protected:

        // Command string
        const cmd_t m_command;

        // Input string
        const std::string m_input_string;

        // Output buffer
        std::string m_output_buffer;
};

class ForkedProcessHandler : public AbstractProcessHandler
{

    public:

        // Construct from command string and input string
        // This will fork a process whose standard input and output is
        // redirected to pipes. The input string is immediately written to the
        // write pipe.
        ForkedProcessHandler(const cmd_t& command, const std::string& input_string);

        // Destructor will wait on forked process and close read pipe
        virtual ~ForkedProcessHandler() override;

        // Terminate simulation prematurely by sending SIGTERM first and
        // SIGKILL if process does not respond.
        virtual void terminate() override;

        // Poll read pipe for any outstanding data and check whether forked
        // process has finished
        virtual bool isDone() override;

    private:

        // Process id of simulator
        pid_t m_child_pid;

        // File descriptors for pipes
        int m_pipe_write_fd;
        int m_pipe_read_fd;

        // Read pipe status flag
        bool m_read_done = false;
};

class MPIProcessHandler : public AbstractProcessHandler
{

    public:

        // Construct from command string and input string
        // This will spawn an MPI process that communicates with the
        // MPIProcessHandler via the intercommunicator created by
        // MPI_Comm_spawn. The input string is immediately sent to the spawned
        // MPI process.
        MPIProcessHandler(const cmd_t& command, const std::string& input_string);

        // Destructor waits for the spawned MPI process to terminate and
        // disconnects the intercommunicator with the spawned MPI process.
        virtual ~MPIProcessHandler() override;

        // "Terminate" simulation by waiting for message from the spawned MPI
        // process. Note that the MPI standard does not provide any
        // functionality to wait for the spawned MPI process to finish, hence
        // that are no guarantees the spawned MPI process has actually
        // finished. The assumption here is that the spawned MPI process
        // terminates shortly after sending out its message.
        virtual void terminate() override;

        // Probe intercommunicator for message and receive message if it is
        // ready.
        virtual bool isDone() override;

    private:

        // Intercomm with child
        MPI::Intercomm m_child_comm;

        // Flag for receiving result
        bool m_result_received = false;
};

#endif // PROCESSHANDLER_H
