#include <string>
#include <vector>
#include <istream>
#include <sstream>
#include <stdexcept>

#include "read_input.h"

#ifndef NDEBUG
#include <iostream>
#endif

/***** Rejection method *****/
namespace rejection {

const int NUM_LINES = 4;
const int EPSILON = 0;
const int SIMULATOR = 1;
const int PARAMETER_NAMES = 2;
const int PRIOR_SAMPLER = 3;

void read_input(std::istream& istrm, input_t& input_obj) {

    using namespace std;

    // Read lines
    vector<string> lines;
    read_lines(istrm, NUM_LINES, lines);

    // Get epsilon
    input_obj.epsilon = lines[EPSILON];

    // Get simulator
    input_obj.simulator = lines[SIMULATOR];

    // Parse parameter names
    parse_csv_list(lines[PARAMETER_NAMES], input_obj.parameter_names);

    // Get prior sampler
    input_obj.prior_sampler = lines[PRIOR_SAMPLER];
}

}

/***** MCMC method *****/
namespace mcmc {

const int NUM_LINES = 7;
const int EPSILON = 0;
const int SIMULATOR = 1;
const int PARAMETER_NAMES = 2;
const int INITIALIZER = 3;
const int PROPOSER = 4;
const int PRIOR_PDF = 5;
const int PROPOSAL_PDF = 6;

void read_input(std::istream& istrm,
                input_t& input_obj) {

    using namespace std;

    // Read lines
    vector<string> lines;
    read_lines(istrm, NUM_LINES, lines);

    // Get epsilon
    input_obj.epsilon = lines[EPSILON];

    // Get simulator
    input_obj.simulator = lines[SIMULATOR];

    // Parse parameter names
    parse_csv_list(lines[PARAMETER_NAMES], input_obj.parameter_names);

    // Get rest
    input_obj.initializer = lines[INITIALIZER];
    input_obj.proposer = lines[PROPOSER];
    input_obj.prior_pdf = lines[PRIOR_PDF];
    input_obj.proposal_pdf = lines[PROPOSAL_PDF];
}

}

/***** SMC method *****/
namespace smc {

const int NUM_LINES = 7;
const int EPSILONS = 0;
const int SIMULATOR = 1;
const int PARAMETER_NAMES = 2;
const int PRIOR_SAMPLER = 3;
const int PERTURBER = 4;
const int PRIOR_PDF = 5;
const int PERTURBATION_PDF = 6;

void read_input(std::istream& istrm,
                input_t& input_obj) {

    using namespace std;

    // Read lines
    vector<string> lines;
    read_lines(istrm, NUM_LINES, lines);

    // Parse epsilons
    parse_csv_list(lines[EPSILONS], input_obj.epsilons);

    // Get simulator
    input_obj.simulator = lines[SIMULATOR];

    // Parse parameter names
    parse_csv_list(lines[PARAMETER_NAMES], input_obj.parameter_names);

    // Get rest
    input_obj.prior_sampler = lines[PRIOR_SAMPLER];
    input_obj.perturber = lines[PERTURBER];
    input_obj.prior_pdf = lines[PRIOR_PDF];
    input_obj.perturbation_pdf = lines[PERTURBATION_PDF];
}

}

/***** Parameter sweep *****/
namespace sweep {

const int NUM_LINES = 3;
const int SIMULATOR = 0;
const int PARAMETER_NAMES = 1;
const int GENERATOR = 2;

void read_input(std::istream& istrm, input_t& input_obj) {

    using namespace std;

    // Read lines
    vector<string> lines;
    read_lines(istrm, NUM_LINES, lines);

    // Get simulator
    input_obj.simulator = lines[SIMULATOR];

    // Parse parameter names
    parse_csv_list(lines[PARAMETER_NAMES], input_obj.parameter_names);

    // Get generator
    input_obj.generator = lines[GENERATOR];
}

}

void parse_csv_list(const std::string& csv_list,
                 std::vector<std::string>& items) {

    using namespace std;

    // Copy comma-separated list and turn into space-separated list
    string item, list_copy(csv_list);
    int num_items = 0;

    for (auto it = list_copy.begin();
         it != list_copy.end() ; it++) {
        if (*it == ',') {
            *it = ' ';
            num_items++;
        }
    }
    num_items++;

    stringstream sstrm;
    sstrm.str(list_copy);

    // Read items
    items.clear();
    for (int i = 0; i < num_items; i++) {
        sstrm >> item;

        if (sstrm.fail()) {
            runtime_error e("error occurred while reading items");
            throw e;
        }

        items.push_back(item);
    }
}

void read_lines(std::istream& istrm, const int num_lines,
                std::vector<std::string>& lines) {

    using namespace std;

    vector<string> raw_lines;
    // Read lines, ignoring blank lines and comments
    for (int lines_left = num_lines; lines_left > 0; lines_left--) {
        string line;
        getline(istrm, line);

        if (istrm.fail()) {
            runtime_error e("an error occurred while reading the input");
            throw e;
        }

        // Skip comments and blank lines
        if ( (line[0] == '#') || (line.size() == 0) ) {
            lines_left++; continue;
        }

        // If line ends with backslash, do not count it
        if ( line.back() == '\\' )
            lines_left++;

        raw_lines.push_back(line);
    }

    // Paste together lines linked with backslash
    for (auto it = raw_lines.begin(); it != raw_lines.end(); it++) {
        string line;

        while ( it->back() == '\\' ) {
            it->back() = '\n';
            line += *it;
            it++;
            if (it == raw_lines.end()) {
                runtime_error e("an error occurred while reading the input");
                throw e;
            }
        }

        line += *it;
        lines.push_back(line);
    }

    if ( lines.size() != num_lines ) {
        runtime_error e("an error occurred while reading the input");
        throw e;
    }
}
