#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <map>
#include <assert.h>

#include "utils.h"

bool is_whitespace(const char letter)
{
    return (letter == ' ')
        || (letter == '\t')
        || (letter == '\n');
}

std::vector<std::string> parse_command_tokens(const std::string& raw_command)
{
    // Define states of finite state machine
    enum state_t { start, unquoted, quote_next_letter, singly_quoted,
        doubly_quoted };

    // Initialize
    state_t state = start;
    std::vector<std::string> cmd_tokens;
    std::stringstream token_strm;

    // Iterate over raw_command string
    for (auto it = raw_command.cbegin(); it != raw_command.cend(); it++)
    {
        // Get current letter
        char letter = *it;

        switch (state)
        {

            // Starting state
            case start:

                // If whitespace, stay in starting state
                while (is_whitespace(letter))
                {
                    it++;
                    if (it == raw_command.cend()) goto endloop;
                    letter = *it;
                }

                // If backslash, transition to quote_next_letter
                if (letter == '\\')
                {
                    state = quote_next_letter;
                }

                // If single quote, transition to singly_quoted
                else if (letter == '\'')
                {
                    state = singly_quoted;
                }

                // If double quote, transition to doubly_quoted
                else if (letter == '\"')
                {
                    state = doubly_quoted;
                }

                // If normal character, add character
                // and transition to unquoted
                else
                {
                    token_strm << letter;
                    state = unquoted;
                }

                break;

            // Quote next letter state
            case quote_next_letter:

                // Add next character literally
                token_strm << letter;

                // Transition to unquoted
                state = unquoted;

                break;

            // Singly quoted state
            case singly_quoted:

                // If any other character than single quote,
                // add character and stay in singly quoted state
                while (letter != '\'')
                {
                    token_strm << letter;
                    it++;
                    if (it == raw_command.cend()) goto endloop;
                    letter = *it;
                }

                // If single quote, transition to unquoted
                state = unquoted;

                break;

            // Double quoted state
            case doubly_quoted:

                // If any other character than double quote,
                // add character and stay in doubly quoted state
                while (letter != '\"')
                {
                    token_strm << letter;
                    it++;
                    if (it == raw_command.cend()) goto endloop;
                    letter = *it;
                }

                // If double quote, transition to unquoted
                state = unquoted;

                break;

            // Unquoted state
            case unquoted:

                // If normal character, add and stay in
                // unquoted state
                while ( !is_whitespace(letter) &&
                        (letter != '\\') &&
                        (letter != '\'') &&
                        (letter != '\"') )
                {
                    token_strm << letter;
                    it++;
                    if (it == raw_command.cend()) goto endloop;
                    letter = *it;
                }

                // If whitespace, push token and
                // transition to starting state
                if (is_whitespace(letter))
                {
                    cmd_tokens.push_back(token_strm.str());
                    token_strm.str("");
                    state = start;
                }

                // If backslash, transition
                // to quote_next_letter state
                else if (letter == '\\')
                    state = quote_next_letter;

                // If single quote, transition
                // to singly quoted state
                else if (letter == '\'')
                    state = singly_quoted;

                // If double quote, transition
                // to doubly quoted state
                else if (letter == '\"')
                    state = doubly_quoted;

                break;
        }
    }

endloop:

    // Check for unfinished quotations
    if ( (state == singly_quoted) || (state == doubly_quoted) ||
            (state == quote_next_letter))
    {
        std::string error_msg;
        error_msg += "Encountered unfinished quotations "
            "while parsing command: ";
        error_msg += raw_command;
        throw std::runtime_error(error_msg);
    }

    // Push back last token
    if (state == unquoted)
    {
        cmd_tokens.push_back(token_strm.str());
    }

    return cmd_tokens;
}

std::vector<std::string> parse_tokens(const std::string& str,
        const std::string& delimiters)
{
    std::vector<std::string> str_vector;

    char *c_str = strdup(str.c_str());
    char *pch = strtok(c_str, delimiters.c_str());

    while (pch != nullptr)
    {
        str_vector.push_back(pch);
        pch = strtok(nullptr, delimiters.c_str());
    }

    free(c_str);

    return str_vector;
}

std::map<std::string, std::string> parse_key_value_pairs(const std::string& str)
{
    // Define states of finite state machine
    enum state_t { read_key, read_value, quote_next_key_letter,
        quote_next_value_letter };

    // Initialize
    state_t state = read_key;
    std::map<std::string, std::string> dict;
    std::stringstream key_strm, value_strm;

    // Iterate over str
    for (auto it = str.cbegin(); it != str.cend(); it++)
    {
        // Get current letter
        char letter = *it;

        switch (state)
        {

            // Read key state
            case read_key:

                // If backslash, transition to quote_next_key_letter
                if (letter == '\\')
                {
                    state = quote_next_key_letter;
                }

                // If equal sign, transition to read_value
                else if (letter == '=')
                {
                    state = read_value;
                }

                // If normal character, push character to key_strm and remain
                // in read_key
                else
                {
                    key_strm << letter;
                }

                break;

            // Read value state
            case read_value:

                // If backslash, transition to quote_next_value_letter
                if (letter == '\\')
                {
                    state = quote_next_value_letter;
                }

                // If semicolon sign, push key-value pair, clear key_strm and
                // value_strm, and transition to read_key
                else if (letter == ';')
                {
                    dict[key_strm.str()] = value_strm.str();

                    key_strm.str("");
                    key_strm.clear();
                    value_strm.str("");
                    value_strm.clear();

                    state = read_key;
                }

                // If normal character, push character to value_strm and remain
                // in read_value
                else
                {
                    value_strm << letter;
                }

                break;

            // Quote next key letter state
            case quote_next_key_letter:

                // Add next character literally
                key_strm << letter;

                // Transition to read key state
                state = read_key;

                break;

            // Quote next value letter state
            case quote_next_value_letter:

                // Add next character literally
                value_strm << letter;

                // Transition to read value state
                state = read_value;

                break;

        }
    }

    // Check for unfinished quotations
    if (state == quote_next_value_letter)
    {
        std::string error_msg;
        error_msg += "Encountered unfinished quotations "
            "while parsing key-value pairs: ";
        error_msg += str;
        throw std::runtime_error(error_msg);
    }

    // Check for incomplete key-value pairs
    if ( (state == read_key) || (state == quote_next_key_letter) )
    {
        std::string error_msg;
        error_msg += "Encountered incomplete key-value pairs "
            "while parsing key-value pairs: ";
        error_msg += str;
        throw std::runtime_error(error_msg);
    }

    // Sanity check: final state should be read_value
    assert(state == read_value);

    // Push last key-value pair
    dict[key_strm.str()] = value_strm.str();

    return dict;
}
