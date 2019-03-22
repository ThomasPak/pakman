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
