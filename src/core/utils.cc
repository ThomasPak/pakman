#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <string.h>

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
    enum state_t { start, unquoted, singly_quoted, doubly_quoted };

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

                // If single quote, transition to singly_quoted
                if (letter == '\'')
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
    if ( (state == singly_quoted) || (state == doubly_quoted) )
    {
        std::string error_msg;
        error_msg += "Encountered unfinished quotations while parsing command: ";
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
