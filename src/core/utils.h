#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

/** @file utils.h
 *
 * Utility functions.
 */

/** Check if letter is a space, tab or newline.
 *
 * @param letter  letter to check.
 *
 * @return whether letter is space, tab or newline.
 */
bool is_whitespace(const char letter);

/** Parses raw command string.
 *
 * @param raw_command  raw command string.
 *
 * @return vector of command tokens.
 */
std::vector<std::string> parse_command_tokens(const std::string& raw_command);

/** Splits string into tokens with given delimiters.
 *
 * @param str  raw string.
 * @param delimiters  string containing all delimiters.
 *
 * @return vector of tokens.
 */
std::vector<std::string> parse_tokens(const std::string& str,
        const std::string& delimiters = " ");

#endif // UTILS_H
