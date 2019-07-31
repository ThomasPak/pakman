#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

#include <assert.h>

#include "system/system_call.h"
#include "interface/protocols.h"

#include "smc_weight.h"

double smc_weight(const Command& perturbation_pdf,
                  const double prmtr_prior_pdf,
                  const int t,
                  const std::vector<Parameter>& prmtr_accepted_old,
                  const std::vector<double>& weights_old,
                  const Parameter& prmtr_perturbed)
{
    // Sanity check: prmtr_accepted_old and weights_old should have the same
    // size
    assert(prmtr_accepted_old.size() == weights_old.size());

    // If in generation 0, return uniform weight
    if (t == 0)
        return 1.0 / ((double) prmtr_accepted_old.size());

    // Get perturbation pdf
    std::vector<double> perturbation_pdf_old =
        get_perturbation_pdf(perturbation_pdf, t, prmtr_perturbed,
                prmtr_accepted_old);

    // Compute denominator
    double denominator = 0.0;

    try
    {
        for (int i = 0; i < weights_old.size(); i++)
            denominator += weights_old[i] * perturbation_pdf_old.at(i);
    }
    catch (const std::out_of_range& e)
    {
        std::string error_msg;
        error_msg += "Out of range: ";
        error_msg += e.what();
        error_msg += '\n';
        error_msg += "Perturbation_pdf did not output enough parameters";
        throw std::runtime_error(error_msg);
    }

    // Return weight
    return prmtr_prior_pdf / denominator;
}
