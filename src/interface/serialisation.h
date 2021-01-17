#ifndef SERIALISATION_H
#define SERIALISATION_H

#include <ostream>

#include "core/Command.h"
#include "core/TaskHandler.h"

#include "LineString.h"

/** @file serialisation.h
 *
 * This file contains functions to serialise scalar values, vector values, and
 * the whole Pakman state.
 */

/** Serialise scalar value
 *
 * @param key  identifier of serialised value
 * @param value  value to serialise
 * @param out  output stream to write to
 */
template <typename value_type>
void serialise_scalar_value(const LineString& key, const value_type& value,
        std::ostream& out)
{
    out << key.str() << ":" << value << std::endl;
}

/** Serialise scalar bool value
 *
 * @param key  identifier of serialised value
 * @param value  bool to serialise
 * @param out  output stream to write to
 */
template <>
void serialise_scalar_value(const LineString& key, const bool& value,
        std::ostream& out);

/** Serialise scalar string value
 *
 * In order to serialise a string value, we add a field containing the number
 * of characters in the string.
 *
 * @param key  identifier of serialised value
 * @param value  string to serialise
 * @param out  output stream to write to
 */
template <>
void serialise_scalar_value(const LineString& key, const std::string& value,
        std::ostream& out);

/** Serialise scalar Command value
 *
 * We serialise Command objects by serialising the raw command string.
 *
 * @param key  identifier of serialised value
 * @param value  Command to serialise
 * @param out  output stream to write to
 */
template <>
void serialise_scalar_value(const LineString& key, const Command& value,
        std::ostream& out);

/** Serialise scalar TaskHandler value
 *
 * We serialise TaskHandler objects by serialising its
 * input string, output string, and error code.
 *
 * @param key  identifier of serialised value
 * @param value  TaskHandler to serialise
 * @param out  output stream to write to
 */
template <>
void serialise_scalar_value(const LineString& key, const TaskHandler& value,
        std::ostream& out);

/** Serialise vector
 *
 * @param key  identifier of serialised vector
 * @param values  values to serialise
 * @param out  output stream to write to
 */
template <typename value_type>
void serialise_vector(const LineString& key,
        const std::vector<value_type>& values,
        std::ostream& out)
{
    // Print key and number of elements
    out << key.str() << ":" << values.size() << std::endl;

    // Print each element with key_n
    for (unsigned idx = 0; idx < values.size(); ++idx)
    {
        serialise_scalar_value<value_type>(key.str() + "_" + std::to_string(idx),
                values[idx], out);
    }
}

/** Overload << operator for LineString
 *
 * @param out  output stream
 * @param line_string  line string
 *
 * @return output stream
 */
std::ostream& operator<<(std::ostream& out, const LineString& line_string);

#endif // SERIALISATION_H
