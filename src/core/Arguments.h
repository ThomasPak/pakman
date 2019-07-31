#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <string>
#include <vector>
#include <unordered_map>

class LongOptions;

/** A class for representing command-line arguments.
 *
 * An Arguments object takes a LongOptions instance and uses it to parse the
 * command-line arguments, as given by argc and argv.  The class uses
 * `getopt_long()` to parse the command-line arguments.
 *
 * The optional and positional arguments can then be accessed with the methods
 * optionalArgument() and positionalArgument(), respectively.
 */

class Arguments
{
    public:

        /** Construct from LongOptions instance, argc and argv.
         *
         * The constructor uses `getopt_long()` internally, starting from the
         * current value of optind.  After construction, the value of optind is
         * equal to argc.
         *
         * @param long_options  instance of LongOptions to determine valid
         * options.
         * @param argc  number of command-line arguments.
         * @param argv  array of command-line arguments.
         */
        Arguments(const LongOptions& long_options, int argc, char *argv[]);

        /** Default destructor does nothing. */
        ~Arguments() = default;

        /** Check whether optional argument was set.
         *
         * @param option_name  name of optional argument.
         *
         * @return whether optional argument was set.
         */
        bool isOptionalArgumentSet(const std::string& option_name) const;

        /** Return value of optional argument.
         *
         * @param option_name  name of optional argument.
         *
         * @return value of optional argument.  Empty string if optional
         * argument was not set.
         */
        std::string optionalArgument(const std::string& option_name) const;

        /** @return number of positional arguments. */
        int numberOfPositionalArguments() const;

        /** Get positional argument from index.
         *
         * @param index  index of positional argument
         *
         * @return positional argument at the given index.
         */
        std::string positionalArgument(int index) const;

    private:

        // Optional arguments are internally represented by a unordered_map
        std::unordered_map<std::string, std::string> m_optional_args;

        // Positional arguments are internally represented by a vector
        std::vector<std::string> m_positional_args;
};

#endif // ARGUMENTS_H
