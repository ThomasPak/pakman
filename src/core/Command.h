#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>

/** A class for representing shell commands.
 *
 * Commands take a raw command string and convert it to an array of command
 * tokens using the function parse_command_tokens().
 *
 * The array of command tokens, to be used in system functions such as exec(),
 * can be accessed with the function argv().
 */

class Command
{
    public:

        /** Default constructor. */
        Command();

        /** Construct from raw command string.
         *
         * @param raw_command  C++-style string containing raw command.
         */
        Command(const std::string& raw_command);

        /** Construct from raw command string.
         *
         * @param raw_command  C-style string containing raw command.
         */
        Command(const char raw_command[]);

        /** Copy constructor.
         *
         * @param command  source Command object.
         */
        Command(const Command& command);

        /** Move constructor.
         *
         * @param command  source Command object.
         */
        Command(Command&& command);

        /** Copy-assignment constructor.
         *
         * @param command  source Command object.
         *
         * @return reference to copy-assigned Command object.
         */
        Command& operator=(const Command& command);

        /** Move-assignment constructor.
         *
         * @param command  source Command object.
         *
         * @return reference to move-assigned Command object.
         */
        Command& operator=(Command&& command);

        /** Destructor. */
        ~Command();

        /** @return raw command string. */
        const std::string& str() const;

        /** @return argv.
         *
         * @warning Do NOT call free on the returned pointer or any of the
         * pointers it points to.  Else, there WILL be pain.
         */
        char** argv() const;

        /** @return whether argv[0] is a valid executable. */
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
