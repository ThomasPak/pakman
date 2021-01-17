#include "serialisation.h"

template <>
void serialise_scalar_value(const LineString& key, const bool& value,
        std::ostream& out)
{
    // Store format flags
    auto fmtflags = out.flags();

    // Serialise bool value
    out << std::boolalpha << key.str() << ":" << value << std::endl;

    // Restore format flags
    out.flags(fmtflags);
}

template <>
void serialise_scalar_value(const LineString& key, const std::string& value,
        std::ostream& out)
{
    out << key.str() << ":" << value.size() << ":" << value << std::endl;
}

template <>
void serialise_scalar_value(const LineString& key, const Command& value,
        std::ostream& out)
{
    serialise_scalar_value(key, value.str(), out);
}

template <>
void serialise_scalar_value(const LineString& key,
        const TaskHandler& value, std::ostream& out)
{
    serialise_scalar_value(key.str() + ":m_input_string",
            value.getInputString(), out);
    serialise_scalar_value(key.str() + ":m_output_string",
            value.getOutputString(), out);
    serialise_scalar_value(key.str() + ":m_error_code",
            value.getErrorCode(), out);
}

std::ostream& operator<<(std::ostream& out, const LineString& line_string)
{
    out << line_string.str();
    return out;
}
