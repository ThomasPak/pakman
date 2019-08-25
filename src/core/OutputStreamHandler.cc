#include <string>
#include <fstream>

#include "core/common.h"

#include "OutputStreamHandler.h"

// Initialise OutputStreamHandler's static data member
OutputStreamHandler* OutputStreamHandler::s_instance = nullptr;

// Return singleton instance
OutputStreamHandler* OutputStreamHandler::instance()
{
    if (s_instance == nullptr)
        s_instance = new OutputStreamHandler(g_output_file);

    return s_instance;
}

// Close file if a filename was given
void OutputStreamHandler::destroy()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

// Return reference to output stream
std::ostream& OutputStreamHandler::getOutputStream()
{
    return *m_p_output_stream;
}

// Private default constructor
OutputStreamHandler::OutputStreamHandler(const std::string& filename)
    : m_filename(filename)
{
    // Make output file if a filename was given (default is &std::cout)
    if (!m_filename.empty())
        m_p_output_stream = new std::ofstream(filename);
}

// Private destructor
OutputStreamHandler::~OutputStreamHandler()
{
    // Close output file if a filename was given
    if (!m_filename.empty())
        delete m_p_output_stream;
}
