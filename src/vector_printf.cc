#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

#include "vector_printf.h"

std::string vector_printf(const std::string& format,
                          const std::vector<std::string>& args,
                          const std::string& token)
{

    using namespace std;

    size_t pos = 0, new_pos = 0;
    stringstream sstrm;

    for (auto it = args.cbegin(); it != args.cend(); it++)
    {

        new_pos = format.find(token, pos);

        if (new_pos == string::npos)
        {
            runtime_error e("too many arguments provided");
            throw e;
        }

        sstrm << format.substr(pos, new_pos - pos);
        sstrm << *it;

        pos = new_pos + token.size();
    }

    string tail = format.substr(pos, string::npos);

    if (tail.find(token) != string::npos)
    {
        runtime_error e("not enough arguments provided");
        throw e;
    }

    sstrm << tail;
    return sstrm.str();
}
