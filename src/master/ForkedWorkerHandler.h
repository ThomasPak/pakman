#ifndef FORKEDWORKERHANDLER_H
#define FORKEDWORKERHANDLER_H

#include <string>

#include "AbstractWorkerHandler.h"

/** A class for representing forked Workers.
 *
 * Forked Workers are spawned using a `fork()`--`exec()` pattern.  This is the
 * default choice for instantiating simulators and is analogous to how
 * SerialMaster launches simulations.
 */

class ForkedWorkerHandler : public AbstractWorkerHandler
{

    public:

        /** Construct from simulator string and input string.
         *
         * The constructor will fork a process whose standard input and output
         * is redirected to a write and a read pipe, respectively..  The input
         * string is immediately written to the write pipe.
         *
         * @param simulator  command to run simulation.
         * @param input_string  input string to simulator.
         */
        ForkedWorkerHandler(const Command& simulator, const std::string&
                input_string);

        /** Destructor.
         *
         * The destructor waits on the forked process and closes the read
         * pipe.
         */
        virtual ~ForkedWorkerHandler() override;

        /** @return whether Worker has finished.
         *
         * Poll read pipe for any outstanding output and check whether forked
         * process has finished.
         */
        virtual bool isDone() override;

    private:

        /** Terminate active Worker with system signals.
         *
         * Terminate simulation by sending `SIGTERM` first, followed by
         * `SIGKILL` if process does not respond.
         */
        void terminate();

        // Process id of simulator
        pid_t m_child_pid;

        // File descriptors for pipes
        int m_pipe_write_fd;
        int m_pipe_read_fd;

        // Read pipe status flag
        bool m_read_done = false;
};

#endif // FORKEDWORKERHANDLER_H
