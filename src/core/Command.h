#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>

class Command
{
    public:

        // Default constructor
        Command();

        // Construct from string
        Command(const std::string& raw_command);

        // Construct from c-style string
        Command(const char raw_command[]);

        // Copy constructor
        Command(const Command& command);

        // Move constructor
        Command(Command&& command);

        // Copy-assignment constructor
        Command& operator=(const Command& command);

        // Move-assignment constructor
        Command& operator=(Command&& command);

        // Destructor
        ~Command();

        // Return command as string
        const std::string& str() const;

        // Return argv
        // Do NOT call free on the returned pointer or any of the pointers it
        // points to.  Else, there WILL be pain.
        char** argv() const;

        // Checks argv[0] to see if it is executable
        bool isExecutable() const;

    private:

        // Copy command tokens to argv
        void copyCommandTokensToArgv();

        // Free argv
        void freeArgv();

        // Save raw command as string
        std::string m_raw_command;

        // Save command tokens
        std::vector<std::string> m_cmd_tokens;

        // Save parsed command as argv
        char **m_argv;
};

#endif // COMMAND_H
