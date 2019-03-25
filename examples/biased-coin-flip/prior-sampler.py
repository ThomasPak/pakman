from numpy.random import uniform
from sys import argv, stdout, stderr

# Process arguments
if len(argv) != 3:
    stderr.write("Usage: {} Q_LOW Q_HIGH\n"
            "Sample from a uniform distribution with "
            "lower bound Q_LOW and upper bound Q_HIGH\n".format(argv[0]))

    exit(1)

q_low = float(argv[1])
q_high = float(argv[2])

# Sample from uniform distribution
q_sampled = uniform(low=q_low, high=q_high)

# Print sampled parameter
stdout.write("{}\n".format(q_sampled))
