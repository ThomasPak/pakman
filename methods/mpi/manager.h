#ifndef MANAGER_H
#define MANAGER_H

#include <mpi.h>

#include "ProcessHandler.h"
#include "../types.h"

void set_terminate_flag(int signal);
void set_signal_handler();
void receive_raw_input(std::string& raw_input);
void process_signal(const MPI::Status& status);
void check_simulation(AbstractProcessHandler*& sim_handler);
void process_parameter(const MPI::Status& status,
                       const std::string& epsilon,
                       const cmd_t& simulator,
                       AbstractProcessHandler*& sim_handler);
void manager(const std::vector<std::string>& epsilons,
             const cmd_t& simulator);

#endif // MANAGER_H
