#include <string>
#include <vector>
#include <ostream>
#include <sstream>

#include "core/utils.h"
#include "interface/Parameter.h"

#include "write_parameters.h"

void write_parameters(std::ostream& ostrm,
                      const std::vector<std::string>& parameter_names,
                      const std::vector<Parameter>& parameters)
{
    // Print header
    {
        std::stringstream sstrm;

        for (auto prmtr_name = parameter_names.cbegin();
             prmtr_name != parameter_names.cend(); prmtr_name++)
            sstrm << *prmtr_name << ",";

        sstrm.seekp(sstrm.tellp() - (std::streamoff) 1);
        sstrm << std::endl;

        ostrm << sstrm.str();
    }

    // Print accepted parameter sets
    for (auto set = parameters.cbegin(); set != parameters.cend(); set++)
    {

        std::stringstream sstrm;

        std::vector<std::string> prmtr_elements =
            parse_tokens(set->str(), " \n\t");

        for (auto element = prmtr_elements.cbegin();
             element != prmtr_elements.cend(); element++)
        {
            sstrm << *element << ",";
        }

        sstrm.seekp(sstrm.tellp() - (std::streamoff) 1);
        sstrm << std::endl;

        ostrm << sstrm.str();
    }
}
