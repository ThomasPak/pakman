#include <string>
#include <vector>
#include <istream>
#include <sstream>
#include <stdexcept>

#include "read_input.h"

std::vector<std::string> parse_csv_list(const std::string& csv_list)
{
    // Initialize items
    std::vector<std::string> items;

    // Copy comma-separated list and turn into space-separated list
    std::string item, list_copy(csv_list);
    int num_items = 0;

    for (auto it = list_copy.begin();
         it != list_copy.end() ; it++)
    {
        if (*it == ',')
        {
            *it = ' ';
            num_items++;
        }
    }
    num_items++;

    std::stringstream sstrm;
    sstrm.str(list_copy);

    // Read items
    items.clear();
    for (int i = 0; i < num_items; i++)
    {
        sstrm >> item;

        if (sstrm.fail())
        {
            std::runtime_error e("error occurred while reading items");
            throw e;
        }

        items.push_back(item);
    }

    return items;
}

std::vector<std::string> read_lines(std::istream& istrm, const int num_lines)
{
    // Initialize lines and raw_lines
    std::vector<std::string> lines;
    std::vector<std::string> raw_lines;

    // Read lines, ignoring blank lines and comments
    for (int lines_left = num_lines; lines_left > 0; lines_left--)
    {
        std::string line;
        getline(istrm, line);

        if (istrm.fail())
        {
            std::runtime_error e("an error occurred while reading the input");
            throw e;
        }

        // Skip comments and blank lines
        if ( (line[0] == '#') || (line.size() == 0) )
        {
            lines_left++; continue;
        }

        // If line ends with backslash, do not count it
        if ( line.back() == '\\' )
            lines_left++;

        raw_lines.push_back(line);
    }

    // Paste together lines linked with backslash
    for (auto it = raw_lines.begin(); it != raw_lines.end(); it++)
    {
        std::string line;

        while ( it->back() == '\\' )
        {
            it->back() = '\n';
            line += *it;
            it++;
            if (it == raw_lines.end())
            {
                std::runtime_error e("an error occurred while reading the input");
                throw e;
            }
        }

        line += *it;
        lines.push_back(line);
    }

    if ( lines.size() != num_lines )
    {
        std::runtime_error e("an error occurred while reading the input");
        throw e;
    }

    return lines;
}
