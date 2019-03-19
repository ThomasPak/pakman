#ifndef PERSISTENTMPIWORKERHANDLER_H
#define PERSISTENTMPIWORKERHANDLER_H

#include <string>

#include <mpi.h>

#include "types.h"
#include "AbstractWorkerHandler.h"

class PersistentMPIWorkerHandler : public AbstractWorkerHandler
{

    public:

        // Construct from command string and input string
        //
        // If it is the first time that PersistentMPIWorkerHandler is
        // constructed, the constructor will spawn an MPI process that
        // communicates with the PersistentMPIWorkerHandler via the
        // intercommunicator created by MPI_Comm_spawn.  This "persistent" MPI
        // child process will continue to run after this object is destroyed.
        //
        // Else, the object will communicate with the persistent, already
        // existing MPI child process.
        //
        // The input string is sent to persistent MPI child process.
        PersistentMPIWorkerHandler(const cmd_t& command, const std::string& input_string);

        // Destructor waits for the persistent MPI child process to finish and
        // send its output string and error code if it has not yet done so.
        //
        // The assumption here is that the persistent MPI child process does
        // not exit after sending its results, but rather stays alive to accept
        // further input strings.
        virtual ~PersistentMPIWorkerHandler() override;

        // Terminate simulation by waiting for the persistent MPI child process
        // to finish and send its output string and error code if it has not
        // yet done so, and subsequently sending a TERMINATE_WORKER_SIGNAL to
        // the persistent MPI child process.
        //
        // The assumption here is that the persistent MPI child process will
        // then exit.
        virtual void terminate() override;

        // Probe intercommunicator for message and receive message if it is
        // ready.
        virtual bool isDone() override;

        // Static termination function to terminate any Workers that were in an
        // idle state when their Manager terminated.  Since the Managers only
        // call terminate() on busy Workers, any remaining idle persistent
        // Workers stall the termination of the entire program so they need to
        // be explicitly terminated.
        static void terminatePersistent();

    private:

        // Receive message from Worker
        std::string receiveMessage() const;

        // Receive error code from Worker
        int receiveErrorCode() const;

        // Discard results from persistent MPI process
        void discardResults();

        // Intercomm with child
        // This intercommunicator is static so that it survives across multiple
        // instances of PersistentMPIWorkerHandler
        static MPI_Comm m_child_comm;

        // Flag for receiving result
        bool m_result_received = false;
};

#endif // PERSISTENTMPIWORKERHANDLER_H
