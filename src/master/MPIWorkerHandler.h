#ifndef MPIWORKERHANDLER_H
#define MPIWORKERHANDLER_H

#include <string>

#include <mpi.h>

#include "AbstractWorkerHandler.h"

/** A class for representing MPI Workers.
 *
 * MPI Workers are spawned using the MPI function `MPI_Comm_spawn`.  This is
 * necessary when the simulator is built using MPI internally because then it
 * could clash with the Pakman when using the MPIMaster.
 *
 * As opposed to the ForkedWorkerHandler, the MPI child process is not
 * terminated after each simulation.  Rather, the MPI Worker stays alive to
 * accept more simulation tasks.  Each simulation task is represented by a new
 * instance of MPIWorkerHandler.
 *
 * Only when terminate() is called will the MPI Worker process be terminated.
 * Moreover, when a Manager is terminated when it does not have an active MPI
 * Worker, the MPI Worker process is left idling.  Calling terminateStatic()
 * ensures that the remaining MPI Worker processes are also terminated.
 */

class MPIWorkerHandler : public AbstractWorkerHandler
{

    public:

        /** Construct from simulator string and input string
         *
         * If MPIWorkerHandler has not been constructed before, the constructor
         * will spawn an MPI process that communicates with MPIWorkerHandler
         * via the intercommunicator created by MPI_Comm_spawn.  This MPI child
         * process will continue to run after this object is destroyed.
         *
         * If MPIWorkerHandler has been constructed before, the
         * MPIWorkerHandler will communicate with the already existing MPI
         * child process.
         *
         * In both cases, the input string is then sent to the MPI Worker via
         * standard MPI functions on the intercommunicator.
         *
         * @param simulator  command to run simulation.
         * @param input_string  input string to simulator.
         */
        MPIWorkerHandler(const Command& simulator, const std::string&
                input_string);

        /** Destructor.
         *
         * The destructor waits for the MPI Worker to finish its simulation
         * task and send its output string and error code if it has not yet
         * done so.
         *
         * We assume that the MPI child process does not exit after sending its
         * results, but rather stays alive to accept further simulation tasks.
         */
        virtual ~MPIWorkerHandler() override;

        /** Terminate MPI Worker when Pakman terminates.
         *
         * Terminate simulation by waiting for the MPI child process to finish
         * and send its output string and error code if it has not yet done so,
         * and subsequently sending a TERMINATE_WORKER_SIGNAL to the MPI child
         * process.
         *
         * We assume that the MPI child process will then exit.
         */
        virtual void terminate() override;

        /** @return whether Worker has finished.
         *
         * Probe intercommunicator for message and receive message if it is
         * ready.
         */
        virtual bool isDone() override;

        /** Terminate Workers remaining after their Managers have terminated.
         *
         * Since the Managers can only call terminate() on busy Workers, any
         * remaining idle Workers need to be explicitly terminated, else they
         * stall the termination of the entire program.
         */
        static void terminateStatic();

    private:

        // Receive message from Worker
        std::string receiveMessage() const;

        // Receive error code from Worker
        int receiveErrorCode() const;

        // Discard results from MPI process
        void discardResults();

        // Intercomm with child
        // This intercommunicator is static so that it survives across multiple
        // instances of MPIWorkerHandler
        static MPI_Comm m_child_comm;

        // Flag for receiving result
        bool m_result_received = false;
};

#endif // MPIWORKERHANDLER_H
