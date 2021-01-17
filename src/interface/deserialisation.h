#ifndef DESERIALISATION_H
#define DESERIALISATION_H

#include <istream>

#include "core/Command.h"
#include "core/TaskHandler.h"

#include "LineString.h"

/** @file deserialisation.h
 *
 * This file contains functions to deserialise scalar values, vector values,
 * and the whole Pakman state.
 */

/** Read key
 *
 * @param key  identifier of serialised value
 * @param in  input stream to read from
 */
void read_key(const LineString& key, std::istream& in);

/** Read value
 *
 * @param in  input stream to read from
 *
 * @return value string
 */
std::string read_value(std::istream& in);

/** Deserialise scalar value
 *
 * @param key  identifier of serialised value
 * @param in  input stream to read from
 *
 * @return deserialised value
 */
template <typename value_type>
value_type deserialise_scalar_value(const LineString& key, std::istream& in)
{
    // Read key
    read_key(key, in);

    // Read value
    auto value_string = read_value(in);

    // Read out LineString
    value_type val;
    std::istringstream isstr(value_string);
    isstr >> val;

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

/** Deserialise scalar bool value
 *
 * @param key  identifier of serialised value
 * @param in  input stream to read from
 *
 * @return deserialised bool value
 */
template <>
bool deserialise_scalar_value(const LineString& key, std::istream& in);

/** Deserialise scalar string value
 *
 * @param key  identifier of serialised value
 * @param in  input stream to read from
 *
 * @return deserialised string value
 */
template <>
std::string deserialise_scalar_value(const LineString& key, std::istream& in);

/** Deserialise scalar Command value
 *
 * @param key  identifier of serialised value
 * @param in  input stream to read from
 *
 * @return deserialised Command value
 */
template <>
Command deserialise_scalar_value(const LineString& key, std::istream& in);

/** Deserialise scalar TaskHandler value
 *
 * @param key  identifier of serialised value
 * @param in  input stream to read from
 *
 * @return deserialised TaskHandler value
 */
template <>
TaskHandler deserialise_scalar_value(const LineString& key, std::istream& in);

/** Overload >> operator for LineString
 *
 * @param in  input stream
 * @param line_string  line string
 *
 * @return input stream
 */
std::istream& operator>>(std::istream& in, LineString& line_string);

#endif // DESERIALISATION_H
