#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>

typedef std::string cmd_t;

/***** Rejection method *****/
namespace rejection
{

typedef struct
{
    std::string epsilon;
    cmd_t simulator;
    std::vector<std::string> parameter_names;
    cmd_t prior_sampler;
} input_t;

}

/***** MCMC method *****/
namespace mcmc
{

typedef struct
{
    std::string epsilon;
    cmd_t simulator;
    std::vector<std::string> parameter_names;
    cmd_t initializer;
    cmd_t proposer;
    cmd_t prior_pdf;
    cmd_t proposal_pdf;
} input_t;

}

/***** SMC method *****/
namespace smc
{

typedef struct
{
    std::vector<std::string> epsilons;
    cmd_t simulator;
    std::vector<std::string> parameter_names;
    cmd_t prior_sampler;
    cmd_t perturber;
    cmd_t prior_pdf;
    cmd_t perturbation_pdf;
} input_t;

}

/***** parameter sweep *****/
namespace sweep
{

typedef struct
{
    cmd_t simulator;
    std::vector<std::string> parameter_names;
    cmd_t generator;
} input_t;

}

#endif // TYPES_H
