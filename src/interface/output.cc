#include <string>
#include <vector>
#include <ostream>
#include <sstream>

#include "core/utils.h"
#include "interface/types.h"

#include "output.h"

void write_parameters(std::ostream& ostrm,
        const std::vector<ParameterName>& parameter_names,
        const std::vector<Parameter>& parameters)
{
    // Print header
    {
        std::stringstream sstrm;

        for (const ParameterName& parameter_name : parameter_names)
            sstrm << parameter_name.str() << ",";

        sstrm.seekp(sstrm.tellp() - static_cast<std::streamoff>(1));
        sstrm << std::endl;

        ostrm << sstrm.str();
    }

    // Print accepted parameters
    for (const Parameter& parameter : parameters)
    {
        std::vector<std::string> parameter_tokens =
            parse_tokens(parameter.str(), " \n\t");

        std::stringstream sstrm;

        for (int i = 0; i < parameter_tokens.size(); i++)
        {
            sstrm << parameter_tokens[i];

            if (i < parameter_tokens.size() - 1)
                sstrm << ',';
        }

        sstrm << std::endl;

        ostrm << sstrm.str();
    }
}
