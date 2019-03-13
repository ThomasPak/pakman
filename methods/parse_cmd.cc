#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

#include "parse_cmd.h"

enum state_t { start, unquoted, singly_quoted, doubly_quoted };

bool is_whitespace(const char letter)
{
    return (letter == ' ')
        || (letter == '\t')
        || (letter == '\n');
}

void parse_cmd(const cmd_t& cmd, std::vector<std::string>& cmd_tokens)
{

    using namespace std;

    state_t state = start;
    stringstream token_strm;

    cmd_tokens.clear();

    // Iterate over cmd string
    for (auto it = cmd.cbegin(); it != cmd.cend(); it++)
    {

        char letter = *it;

        switch (state)
        {

            // Starting state
            case start:

                // If whitespace, stay in starting state
                while (is_whitespace(letter))
                {
                    it++;
                    if (it == cmd.cend()) goto endloop;
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
                    if (it == cmd.cend()) goto endloop;
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
                    if (it == cmd.cend()) goto endloop;
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
                    if (it == cmd.cend()) goto endloop;
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
        runtime_error e("there were unfinished quotations");
        throw e;
    }

    // Push back last token
    if (state == unquoted)
    {
        cmd_tokens.push_back(token_strm.str());
    }
}
