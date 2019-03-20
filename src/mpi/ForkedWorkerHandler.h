#ifndef FORKEDWORKERHANDLER_H
#define FORKEDWORKERHANDLER_H

#include <string>

#include "core/types.h"

#include "AbstractWorkerHandler.h"

class ForkedWorkerHandler : public AbstractWorkerHandler
{

    public:

        // Construct from command string and input string
        // This will fork a process whose standard input and output is
        // redirected to pipes. The input string is immediately written to the
        // write pipe.
        ForkedWorkerHandler(const cmd_t& command, const std::string& input_string);

        // Destructor will wait on forked process and close read pipe
        virtual ~ForkedWorkerHandler() override;

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

#endif // FORKEDWORKERHANDLER_H
