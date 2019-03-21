#ifndef ABSTRACTWORKERHANDLER_H
#define ABSTRACTWORKERHANDLER_H

#include <string>

#include "core/types.h"

class AbstractWorkerHandler
{

    public:

        // Construct from command string and input string
        AbstractWorkerHandler(const Command& command, const std::string& input_string);

        // Virtual destructor
        virtual ~AbstractWorkerHandler() = default;

        // Virtual function terminate() to prematurely terminate Worker
        virtual void terminate() = 0;

        // Virtual function isDone() to check whether Worker has finished
        virtual bool isDone() = 0;

        // Function getOutput() to get output of finished Worker
        // Running this command before Worker is finished will result in
        // an error, so always check with isDone() first.
        std::string getOutput();

        // Function getErrorCode() to get error code of finished Worker
        // Running this command before Worker is finished will result in
        // an error, so always check with isDone() first.
        int getErrorCode();

    protected:

        // Command string
        const Command m_command;

        // Input string
        const std::string m_input_string;

        // Output buffer
        std::string m_output_buffer;

        // Error code
        int m_error_code = -1;
};

#endif // ABSTRACTWORKERHANDLER_H
