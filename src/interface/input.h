#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <vector>

#include "types.h"

class Command;

/** @file input.h
 *
 * This file contains functions to parse user input from command-line
 * arguments.
 */

/** Parse integer.
 *
 * @param raw_input  raw input string.
 *
 * @return parsed integer.
 */
int parse_integer(const std::string& raw_input);

/** Parse unsigned long integer.
 *
 * @param raw_input  raw input string.
 *
 * @return parsed unsigned long integer.
 */
unsigned long parse_unsigned_long_integer(const std::string& raw_input);

/** Parse Command.
 *
 * @param raw_input  raw input string.
 *
 * @return parsed Command.
 */
Command parse_command(const std::string& raw_input);

/** Parse Epsilon.
 *
 * @param raw_input  raw input string.
 *
 * @return parsed Epsilon.
 */
Epsilon parse_epsilon(const std::string& raw_input);

/** Parse comma-separated epsilon list.
 *
 * @param raw_input  raw input string.
 *
 * @return vector of parsed Epsilons.
 */
std::vector<Epsilon> parse_epsilons(const std::string& raw_input);

/** Parse comma-separated parameter names.
 *
 * @param raw_input  raw input string.
 *
 * @return vector of parsed ParameterNames.
 */
std::vector<ParameterName> parse_parameter_names(const std::string& raw_input);

#endif // INPUT_H
