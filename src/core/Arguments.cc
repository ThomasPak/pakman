#include <string>
#include <stdexcept>

#include <getopt.h>

#include "LongOptions.h"
#include "Arguments.h"

Arguments::Arguments(const LongOptions& long_options, int argc, char *argv[])
{
    // Process optional arguments
    int c;
    while ((c = getopt_long(argc, argv,
                    long_options.getShortOpts(),
                    long_options.getLongOpts(), nullptr)) != -1)
    {
        // Check for error
        if (c == '?')
        {
            std::runtime_error e("an error occured while processing "
                    "arguments using getopt_long");
            throw e;
        }

        // Get long option name and save to m_optional_args
        std::string&& current_option = long_options.getLongOptionName(c);

        m_optional_args[current_option].assign(
                optarg == nullptr ? "" : optarg);
    }

    // Process positional arguments
    for (; optind < argc; optind++)
        m_positional_args.push_back(argv[optind]);
}

// Check if optional argument was set
bool Arguments::isOptionalArgumentSet(const std::string& option_name) const
{
    return m_optional_args.count(option_name) == 1;
}

// Return value of optional argument
// If optional argument does not have value, return empty string
std::string Arguments::optionalArgument(const std::string& option_name) const
{
    return m_optional_args.at(option_name);
}

// Get number of positional arguments
int Arguments::numberOfPositionalArguments() const
{
    return m_positional_args.size();
}

// Get positional argument from index
std::string Arguments::positionalArgument(int index) const
{
    return m_positional_args.at(index);
}
