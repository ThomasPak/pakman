#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <map>

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

/** Splits string into key value pairs.
 *
 * The raw string must contain key value pairs in the format
 * `key1=value1; key2=value2; ...; keyn=valuen`.  The characters `=` and `;`
 * may be escaped with the backslash (`\`) if it appears in the key or the
 * value string.
 *
 * @param str  raw string.
 *
 * @return map of key value pairs.
 */
std::map<std::string, std::string> parse_key_value_pairs(const std::string& str);

#endif // UTILS_H
