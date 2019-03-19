#ifndef RUN_SIMULATION_H
#define RUN_SIMULATION_H

#include <string>
#include <vector>

#include "types.h"
#include "Parameter.h"

std::string simulator_input(const std::string& epsilon,
        const Parameter& prmtr_sample);

int simulation_result(const std::string& output);

int run_simulation(const std::string& epsilon,
                    const cmd_t& simulator,
                    const Parameter& prmtr_sample);

#endif // RUN_SIMULATION_H
