#include <string>
#include <stdexcept>

#include "types.h"
#include "system_call.h"
#include "run_simulation.h"

void simulator_input(const std::string& epsilon,
                     const parameter_t& prmtr_sample,
                     std::string& input) {
    input.clear();
    input += epsilon;
    input += '\n';
    input += prmtr_sample;
    input += '\n';
}

int simulation_result(const std::string& output) {

    using namespace std;

    if (output[0] == '0') // Parameters were rejected
        return 0;
    else if (output[0] == '1') // Parameters were accepted
        return 1;
    else if (output[0] == '2') { // An error occurred
        return 2;
    }
    else { // Invalid output
        std::string error_msg("cannot parse output of simulator: ");
        error_msg += output;
        runtime_error e(error_msg);
        throw e;
    }
}

int run_simulation(const std::string& epsilon,
                    const cmd_t& simulator,
                    const parameter_t& prmtr_sample) {

    using namespace std;

    string input;
    simulator_input(epsilon, prmtr_sample, input);

    string output;
    system_call(simulator, input, output);

    return simulation_result(output);
}
