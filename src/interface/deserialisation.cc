#include <stdexcept>
#include <sstream>

#include "deserialisation.h"

void read_key(const LineString& key, std::istream& in)
{
    // Read until ':'
    std::string read_key;
    std::getline(in, read_key, ':');

    // Check input stream
    if (!in)
    {
        std::string error_msg("deserialisation failed, ");
        error_msg += "could not read key";
        std::runtime_error e(error_msg);
        throw e;
    }

    // Check key
    if (read_key != key.str())
    {
        std::string error_msg("deserialisation failed, read key ('");
        error_msg += read_key;
        error_msg += "') did not match queried key ('";
        error_msg += key.str();
        error_msg += "')";
        std::runtime_error e(error_msg);
        throw e;
    }
}

std::string read_value(std::istream& in)
{
    // Read until newline
    std::string read_value;
    std::getline(in, read_value);

    // Check input stream
    if (!in)
    {
        std::string error_msg("deserialisation failed, ");
        error_msg += "could not read value";
        std::runtime_error e(error_msg);
        throw e;
    }

    return read_value;
}

template <>
bool deserialise_scalar_value(const LineString& key, std::istream& in)
{
    // Read key
    read_key(key, in);

    // Read value
    auto value_string = read_value(in);

    // Deserialise value
    bool val;
    std::istringstream isstr(value_string);
    isstr >> std::boolalpha >> val;

    // Check input stream
    if (!isstr)
    {
        std::string error_msg("deserialisation failed, ");
        error_msg += "could not extract value";
        std::runtime_error e(error_msg);
        throw e;
    }

    return val;
}

template <>
std::string deserialise_scalar_value(const LineString& key, std::istream& in)
{
    // Read key
    read_key(key, in);

    // Read string size
    std::string string_size_string;
    std::getline(in, string_size_string, ':');

    // Check input stream
    if (!in)
    {
        std::string error_msg("deserialisation failed, ");
        error_msg += "could not read string size";
        std::runtime_error e(error_msg);
        throw e;
    }

    // Extract string size
    unsigned string_size;
    std::istringstream isstr(string_size_string);
    isstr >> string_size;

    // Check input stream
    if (!isstr)
    {
        std::string error_msg("deserialisation failed, ");
        error_msg += "could not extract string size";
        std::runtime_error e(error_msg);
        throw e;
    }

    // Extract string, including trailing new newline character
    char buf[string_size + 1];
    in.read(buf, string_size + 1);
    buf[string_size] = '\0';

    if (!in)
    {
        std::string error_msg("deserialisation failed, ");
        error_msg += "could not extract string";
        std::runtime_error e(error_msg);
        throw e;
    }

    std::string val(buf);
    return val;
}

template <>
Command deserialise_scalar_value(const LineString& key, std::istream& in)
{
    // Read string
    auto raw_command_string = deserialise_scalar_value<std::string>(key, in);

    // Construct and return command
    return static_cast<Command>(raw_command_string);
}

template <>
TaskHandler deserialise_scalar_value(const LineString& key, std::istream& in)
{
    read_key(key, in);
    auto input_string = deserialise_scalar_value<std::string>("m_input_string",
            in);

    read_key(key, in);
    auto output_string = deserialise_scalar_value<std::string>(
            "m_output_string", in);

    read_key(key, in);
    auto error_code = deserialise_scalar_value<int>("m_error_code", in);

    TaskHandler task(input_string);

    // If pending task, return without recording output string and error code
    if (output_string.empty() && (error_code == -1))
    {
        return task;
    }

    // If finished task, return after recording output string and error code
    task.recordOutputAndErrorCode(output_string, error_code);

    return task;
}

std::istream& operator>>(std::istream& in, LineString& line_string)
{
    // Extract to string
    std::string str;
    in >> str;

    // Assign LineString
    line_string = LineString(str);

    return in;
}
