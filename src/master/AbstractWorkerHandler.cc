#include <string>

#include <assert.h>

#include "AbstractWorkerHandler.h"

AbstractWorkerHandler::AbstractWorkerHandler(
        const Command& command,
        const std::string& input_string) :
    m_command(command),
    m_input_string(input_string)
{
}

std::string AbstractWorkerHandler::getOutput()
{
    assert(isDone());

    return m_output_buffer;
}

int AbstractWorkerHandler::getErrorCode()
{
    assert(isDone());

    return m_error_code;
}
