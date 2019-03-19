#include <stdexcept>

#include "Parameter.h"

// Construct from lvalue string
Parameter::Parameter(const std::string& raw_parameter) :
    m_parameter(raw_parameter)
{
    removeTrailingNewlines();
    checkForNewlines();
}

// Construct from rvalue string
Parameter::Parameter(std::string&& raw_parameter) :
    m_parameter(std::move(raw_parameter))
{
    removeTrailingNewlines();
    checkForNewlines();
}

// Construct from c-style string
Parameter::Parameter(const char raw_parameter[]) :
    Parameter(static_cast<std::string>(raw_parameter))
{
}

// Remove trailing newlines
void Parameter::removeTrailingNewlines()
{
    // Class invariant: m_parameter has no trailing newlines
    while (m_parameter.back() == '\n')
        m_parameter.pop_back();
}

// Check for newlines
void Parameter::checkForNewlines() const
{
    // Check that there are no newlines anywhere in the parameter
    for (const char& c : m_parameter)
        if (c == '\n')
            throw std::runtime_error("Parameter cannot contain newlines!");
}

// Return string
const std::string& Parameter::str() const
{
    return m_parameter;
}

// Return size
size_t Parameter::size() const
{
    return m_parameter.size();
}
