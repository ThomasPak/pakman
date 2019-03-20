#ifndef LONGOPTIONS_H
#define LONGOPTIONS_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <getopt.h>

class LongOptions
{
    public:

        // Constructor
        LongOptions();

        // Destructor
        ~LongOptions() = default;

        // Add long option
        void add(struct option long_opt);

        // Return longopts
        const struct option* getLongopts() const;

        // Return optstring
        const char* getShortopts() const;

        std::string getLongOptionName(char short_option) const;

    private:

        // Vector of long option structs
        std::vector<struct option> m_long_opts;
        std::unordered_set<std::string> m_long_opts_set;

        // Vector of short option chars
        std::vector<char> m_short_opts;
        std::unordered_set<int> m_short_opts_set;

        // Unordered map of short option char to long option string
        std::unordered_map<char, std::string> m_short_to_long_map;
};

#endif // LONGOPTIONS_H
