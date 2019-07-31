#ifndef LONGOPTIONS_H
#define LONGOPTIONS_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <getopt.h>

/** A class for representing command-line options.
 *
 * The LongOptions class is a utility wrapper for the `struct option` array
 * type defined in getopt.h, and used by `getopt_long()`.
 *
 * Long options are added with add().  The `struct option` array is accessed
 * with getLongOpts(), while the optstring is accessed with getShortOpts().
 */

class LongOptions
{
    public:

        /** Default constructor.
         *
         * The constructor will push the null `struct option` to the `struct
         * option` array so that a valid `struct option` is always maintained.
         */
        LongOptions();

        /** Default destructor does nothing. */
        ~LongOptions() = default;

        /** Add a long option.
         *
         * @param long_opt  long option to add.
         */
        void add(struct option long_opt);

        /** @return long option array. */
        const struct option* getLongOpts() const;

        /** @return optstring. */
        const char* getShortOpts() const;

        /** Get the long option name for a short option.
         *
         * @param short_option short option character.
         *
         * @return long option name.
         */
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
