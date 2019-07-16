from numpy.random import uniform
from sys import argv, stdout, stderr

# Process arguments
if len(argv) != 5:
    stderr.write("Usage: {} BETA_LOW BETA_HIGH GAMMA_LOW GAMMA_HIGH\n"
            "Sample from two uniform distributions; one with lower bound\n"
            "BETA_LOW and upper bound BETA_HIGH, and one with lower bound\n"
            "GAMMA_LOW and upper bound GAMMA_HIGH.\n".format(argv[0]))

    exit(1)

beta_low = float(argv[1])
beta_high = float(argv[2])
gamma_low = float(argv[3])
gamma_high = float(argv[4])

# Sample from uniform distribution for beta
beta_sampled = uniform(low=beta_low, high=beta_high)

# Sample from uniform distribution for gamma
gamma_sampled = uniform(low=gamma_low, high=gamma_high)

# Print sampled parameters
stdout.write("{} {}\n".format(beta_sampled, gamma_sampled))
