#include <stdexcept>

#include "LineString.h"

// Construct from lvalue string
LineString::LineString(const std::string& raw_string) :
    m_line_string(raw_string)
{
    removeTrailingNewlines();
    checkForNewlines();
}

// Construct from rvalue string
LineString::LineString(std::string&& raw_string) :
    m_line_string(std::move(raw_string))
{
    removeTrailingNewlines();
    checkForNewlines();
}

// Construct from c-style string
LineString::LineString(const char raw_string[]) :
    LineString(static_cast<std::string>(raw_string))
{
}

// Remove trailing newlines
void LineString::removeTrailingNewlines()
{
    // Class invariant: m_line_string has no trailing newlines
    while (m_line_string.back() == '\n')
        m_line_string.pop_back();
}

// Check for newlines
void LineString::checkForNewlines() const
{
    // Class invariant: m_line_string has no newlines
    for (const char& c : m_line_string)
        if (c == '\n')
            throw std::runtime_error("LineString cannot contain newlines!");
}

// Return string
const std::string& LineString::str() const
{
    return m_line_string;
}

// Return size
size_t LineString::size() const
{
    return m_line_string.size();
}
