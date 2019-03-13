#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <string>
#include <unordered_map>

#include "LongOptions.h"

class Arguments
{
    public:

        // Construct from LongOptions instance, argc and argv.
        // Constructor uses getopt_long internally, starting from the current
        // value of optind.  After construction, the value of optind is equal
        // to argc
        Arguments(const LongOptions& long_options, int argc, char *argv[]);

        // Destructor
        ~Arguments() = default;

        // Check if optional argument was set
        bool isOptionalArgumentSet(const std::string& option_name) const;

        // Return value of optional argument
        // If optional argument does not have value, return empty string
        std::string optionalArgument(const std::string& option_name) const;

        // Get number of positional arguments
        int numberOfPositionalArguments() const;

        // Get positional argument from index
        std::string positionalArgument(int index) const;

    private:

        // Optional arguments are internally represented by a unordered_map
        std::unordered_map<std::string, std::string> m_optional_args;

        // Positional arguments are internally represented by a vector
        std::vector<std::string> m_positional_args;
};

#endif // ARGUMENTS_H
