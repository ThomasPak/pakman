#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>

#include "Command.h"

/***** Rejection method *****/
namespace rejection
{

typedef struct
{
    std::string epsilon;
    Command simulator;
    std::vector<std::string> parameter_names;
    Command prior_sampler;
} input_t;

}

/***** MCMC method *****/
namespace mcmc
{

typedef struct
{
    std::string epsilon;
    Command simulator;
    std::vector<std::string> parameter_names;
    Command initializer;
    Command proposer;
    Command prior_pdf;
    Command proposal_pdf;
} input_t;

}

/***** SMC method *****/
namespace smc
{

typedef struct
{
    std::vector<std::string> epsilons;
    Command simulator;
    std::vector<std::string> parameter_names;
    Command prior_sampler;
    Command perturber;
    Command prior_pdf;
    Command perturbation_pdf;
} input_t;

}

/***** parameter sweep *****/
namespace sweep
{

typedef struct
{
    Command simulator;
    std::vector<std::string> parameter_names;
    Command generator;
} input_t;

}

#endif // TYPES_H
