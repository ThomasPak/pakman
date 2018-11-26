#ifndef RUN_SIMULATION_H
#define RUN_SIMULATION_H

#include <string>
#include <vector>

#include "types.h"

void simulator_input(const std::string& epsilon,
                     const parameter_t& prmtr_sample,
                     std::string& input);

int simulation_result(const std::string& output);

int run_simulation(const std::string& epsilon,
                    const cmd_t& simulator,
                    const parameter_t& prmtr_sample);

#endif // RUN_SIMULATION_H
