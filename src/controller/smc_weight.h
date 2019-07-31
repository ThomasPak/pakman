#ifndef SMC_WEIGHT_H
#define SMC_WEIGHT_H

#include <vector>
#include <string>

class Command;

double smc_weight(const Command& perturbation_pdf,
                  const double prmtr_prior_pdf,
                  const int t,
                  const std::vector<Parameter>& prmtr_accepted_old,
                  const std::vector<double>& weights_old,
                  const Parameter& prmtr_perturbed);

#endif // SMC_WEIGHT_H
