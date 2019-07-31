#include <stdexcept>

#include <getopt.h>

#include "LongOptions.h"

static const struct option null_long_opt =
{
    nullptr, 0, nullptr, 0,
};

// Constructor
LongOptions::LongOptions()
{
    m_long_opts.push_back(null_long_opt);
    m_short_opts.push_back('\0');
}

// Add long option
void LongOptions::add(struct option long_opt)
{
    // Check if long option is valid
    if ( (long_opt.name == nullptr)
            && (long_opt.has_arg == 0)
            && (long_opt.flag == nullptr)
            && (long_opt.val == 0))
    {
        std::runtime_error e("cannot add empty struct option");
        throw e;
    }

    // Check for duplicates
    if (m_long_opts_set.count(long_opt.name) == 1)
    {
        std::runtime_error e("duplicate long option name");
        throw e;
    }

    if (m_short_opts_set.count(long_opt.val) == 1)
    {
        std::runtime_error e("duplicate short option name");
        throw e;
    }

    // Insert long and short option into sets
    m_long_opts_set.insert(long_opt.name);
    m_short_opts_set.insert(long_opt.val);

    // Insert into map
    m_short_to_long_map[long_opt.val] = long_opt.name;

    // Insert long option into struct option vector
    m_long_opts.pop_back();
    m_long_opts.push_back(long_opt);
    m_long_opts.push_back(null_long_opt);

    // Insert short option into char vector
    m_short_opts.pop_back();
    m_short_opts.push_back(long_opt.val);
    if (long_opt.has_arg == required_argument)
        m_short_opts.push_back(':');
    else if (long_opt.has_arg == optional_argument)
    {
        m_short_opts.push_back(':');
        m_short_opts.push_back(':');
    }
    m_short_opts.push_back('\0');
}

// Return longopts
const struct option* LongOptions::getLongOpts() const
{
    return m_long_opts.data();
}

// Return optstring
const char* LongOptions::getShortOpts() const
{
    return m_short_opts.data();
}

std::string LongOptions::getLongOptionName(char short_option) const
{
    return m_short_to_long_map.at(short_option);
}
