#ifndef MASTER_H
#define MASTER_H

#include <queue>
#include <vector>
#include <set>
#include <string>

#include "../types.h"
#include "../Sampler.h"
#include "ParameterHandler.h"

#include "common.h"

void check_managers(const std::vector<ParameterHandler*>& manager_map,
                    std::set<int>& idle_managers);
void delegate_managers(const AbstractSampler& sampler_obj,
                       const std::string& epsilon,
                       std::vector<ParameterHandler*>& manager_map,
                       std::set<int>& idle_managers,
                       std::queue<ParameterHandler>& prmtr_sampled);
void check_parameters(std::queue<ParameterHandler>& prmtr_sampled,
                      std::vector<parameter_t>& prmtr_accepted);
void send_signal_to_managers(const int signal);

namespace rejection {

void master(const int num_accept, const input_t& input_obj);

}

namespace mcmc {

void master();

}

namespace smc {

void compute_weights(const SMCSampler& smc_sampler,
                     const cmd_t& perturbation_pdf,
                     const int t,
                     const std::vector<parameter_t>::const_iterator& it_end,
                     std::vector<parameter_t>::const_iterator& it,
                     std::vector<double>& weights_new);
void master(const int num_accept, const input_t& input_obj);

}

namespace sweep {

void master(const input_t& input_obj);

}

#endif // MASTER_H
