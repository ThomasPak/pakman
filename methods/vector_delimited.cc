#include <string>
#include <sstream>
#include <vector>

#include "vector_delimited.h"

void vector_delimited(const std::vector<std::string>& str_vector,
                 std::string& delimited_string, const std::string& delimiter) {

    using namespace std;

    stringstream sstrm;

    for (auto it = str_vector.begin();
         it != str_vector.end(); it++)
        sstrm << *it << delimiter;

    delimited_string.assign(sstrm.str());
    delimited_string.pop_back();
}
