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

std::vector<std::string> parse_command(const std::string& raw_command)
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

std::vector<const char*> vector_argv(
        const std::vector<std::string>& cmd_tokens)
{
    // Initialize
    std::vector<const char*> argv;

    // Convert to vector of c-style string
    for (const std::string& it : cmd_tokens)
        argv.push_back(it.c_str());

    // Append nullptr
    argv.push_back(nullptr);

    return argv;
}

void vector_delimited(const std::vector<std::string>& str_vector,
                 std::string& delimited_string, const std::string& delimiter)
{

    using namespace std;

    stringstream sstrm;

    for (auto it = str_vector.begin();
         it != str_vector.end(); it++)
        sstrm << *it << delimiter;

    delimited_string.assign(sstrm.str());
    delimited_string.pop_back();
}

std::string vector_printf(const std::string& format,
                          const std::vector<std::string>& args,
                          const std::string& token)
{

    using namespace std;

    size_t pos = 0, new_pos = 0;
    stringstream sstrm;

    for (auto it = args.cbegin(); it != args.cend(); it++)
    {

        new_pos = format.find(token, pos);

        if (new_pos == string::npos)
        {
            runtime_error e("too many arguments provided");
            throw e;
        }

        sstrm << format.substr(pos, new_pos - pos);
        sstrm << *it;

        pos = new_pos + token.size();
    }

    string tail = format.substr(pos, string::npos);

    if (tail.find(token) != string::npos)
    {
        runtime_error e("not enough arguments provided");
        throw e;
    }

    sstrm << tail;
    return sstrm.str();
}

void vector_strtok(const std::string& str,
                   std::vector<std::string>& str_vector,
                   const std::string& delimiters)
{

    using namespace std;

    char *c_str = strdup(str.c_str()), *pch;

    pch = strtok(c_str, delimiters.c_str());

    str_vector.clear();
    while (pch != NULL)
    {
        str_vector.push_back(pch);
        pch = strtok(NULL, delimiters.c_str());
    }

    free(c_str);
}

char** create_c_argv(const std::vector<std::string>& cmd_tokens)
{
    // Get number of tokens
    int num_tokens = cmd_tokens.size();

    // Allocate pointers to tokens + 1 for nullptr
    char **argv = new char*[num_tokens + 1];

    // Loop, allocate strings and copy
    for (int i = 0; i < num_tokens; i++)
    {
        // Get size of string
        int size = cmd_tokens[i].size();

        // Allocate size of string + 1 for terminating null character
        argv[i] = new char[size + 1];

        // Copy string
        strcpy(argv[i], cmd_tokens[i].c_str());
    }

    // Terminating null pointer
    argv[num_tokens] = nullptr;

    return argv;
}

void free_c_argv(char **argv)
{
    // Loop until nullptr encountered
    int i = 0;
    while (argv[i] != nullptr)
    {
        // Free command token
        delete[] argv[i];
        i++;
    }

    // Free argv
    delete[] argv;
}
