#ifndef MPIWORKERHANDLER_H
#define MPIWORKERHANDLER_H

#include <string>

#include <mpi.h>

#include "AbstractWorkerHandler.h"

class MPIWorkerHandler : public AbstractWorkerHandler
{

    public:

        // Construct from command string and input string
        // This will spawn an MPI process that communicates with the
        // MPIWorkerHandler via the intercommunicator created by
        // MPI_Comm_spawn. The input string is immediately sent to the spawned
        // MPI process.
        MPIWorkerHandler(const Command& command, const std::string& input_string);

        // Destructor waits for the spawned MPI process to terminate and
        // disconnects the intercommunicator with the spawned MPI process.
        virtual ~MPIWorkerHandler() override;

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

        // Receive message from Worker
        std::string receiveMessage() const;

        // Receive error code from Worker
        int receiveErrorCode() const;

        // Intercomm with child
        MPI_Comm m_child_comm;

        // Flag for receiving result
        bool m_result_received = false;
};

#endif // MPIWORKERHANDLER_H
