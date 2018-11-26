#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

#include "types.h"
#include "system_call.h"
#include "smc_weight.h"

#ifndef NDEBUG
#include <iostream>
#endif

double smc_weight(const cmd_t& perturbation_pdf,
                  const double prmtr_prior_pdf,
                  const int t,
                  const std::vector<parameter_t>& prmtr_accepted_old,
                  const std::vector<double>& weights_old,
                  const parameter_t& prmtr_perturbed) {

    using namespace std;

    // If in generation 0, return uniform weight
    if (t == 0)
        return 1.0 / ((double) prmtr_accepted_old.size());

    // Get perturbation pdf
    string input, output;
    input += to_string(t);
    input += '\n';
    input += prmtr_perturbed;
    input += '\n';

    for (auto it = prmtr_accepted_old.begin();
         it != prmtr_accepted_old.end(); it++) {
        input += *it;
        input += '\n';
    }

    system_call(perturbation_pdf, input, output);

    // Compute denominator
    istringstream sstrm(output);
    double denominator = 0.0;

    for (auto it = weights_old.begin();
         it != weights_old.end(); it++) {
        double current_pdf;
        sstrm >> current_pdf;
        if (!sstrm.good()) {
            runtime_error e("an error occured while reading the output of perturbation_pdf");
            throw e;
        }
#ifndef NDEBUG
        cerr << "current_pdf: " << current_pdf << endl;
#endif
        denominator += (*it) * current_pdf;
    }

    // Return weight
    return prmtr_prior_pdf / denominator;
}
