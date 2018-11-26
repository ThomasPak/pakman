#include <string>

#include "system_call.h"
#include "acceptance_probability.h"

#ifndef NDEBUG
#include <iostream>
#endif

double acceptance_probability(const cmd_t& prior_pdf,
                              const cmd_t& proposal_pdf,
                              const parameter_t& old_prmtr,
                              const parameter_t& new_prmtr) {

    using namespace std;

    string old_prior_pdf_str, new_prior_pdf_str,
           old_to_new_proposal_pdf_str, new_to_old_proposal_pdf_str;
    double old_prior_pdf, new_prior_pdf,
           old_to_new_proposal_pdf, new_to_old_proposal_pdf;

    // Get prior pdf
    system_call(prior_pdf, old_prmtr, old_prior_pdf_str);

    system_call(prior_pdf, new_prmtr, new_prior_pdf_str);

    // Get proposal pdf
    string old_to_new_prmtr(old_prmtr);
    old_to_new_prmtr += '\n';
    old_to_new_prmtr += new_prmtr;

    system_call(proposal_pdf, old_to_new_prmtr, old_to_new_proposal_pdf_str);

    string new_to_old_prmtr(new_prmtr);
    new_to_old_prmtr += '\n';
    new_to_old_prmtr += old_prmtr;

    system_call(proposal_pdf, new_to_old_prmtr, new_to_old_proposal_pdf_str);

    // Convert strings to doubles
    old_prior_pdf = stod(old_prior_pdf_str);
    new_prior_pdf = stod(new_prior_pdf_str);
    old_to_new_proposal_pdf = stod(old_to_new_proposal_pdf_str);
    new_to_old_proposal_pdf = stod(new_to_old_proposal_pdf_str);

    // Compute acceptance probability
    double accept_prob = new_prior_pdf * new_to_old_proposal_pdf
                         / ( old_prior_pdf * old_to_new_proposal_pdf );

#ifndef NDEBUG
    cerr << "old_prior_pdf_str: " << old_prior_pdf_str << endl;
    cerr << "new_prior_pdf_str: " << new_prior_pdf_str << endl;

    cerr << "old_to_new_proposal_pdf_str: " << old_to_new_proposal_pdf_str << endl;
    cerr << "new_to_old_proposal_pdf_str: " << new_to_old_proposal_pdf_str << endl;
#endif

    return accept_prob < 1.0 ? accept_prob : 1.0;
}
