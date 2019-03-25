from numpy.random import normal
from sys import argv, stdin, stdout, stderr

# Process arguments
if len(argv) != 2:
    stderr.write("Usage: {} STDEV\n"
            "Perturb given parameter by drawing "
            "from normal distribution\n"
            "centered on the given parameter "
            "and with standard deviation STDEV\n".format(argv[0]))

    exit(1)

stdev = float(argv[1])

# Read generation t and parameter from stdin
stderr.write("Enter t\n")
t = int(stdin.readline())

stderr.write("Enter q\n")
q = float(stdin.readline())

# Print some information
stderr.write("Generation: {}\n".format(t))

# Perturb parameter
q_perturbed = normal(loc=q, scale=stdev)

# Print perturbed parameter
stdout.write("{}\n".format(q_perturbed))
