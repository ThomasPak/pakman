from numpy.random import normal
from sys import argv, stdin, stdout, stderr

# Process arguments
if len(argv) != 3:
    stderr.write("Usage: {} STDEV1 STDEV2\n"
            "Perturb two parameters by drawing "
            "from normal distributions\n"
            "centered on the given parameters "
            "and with standard deviations\n"
            "STDEV1 and STDEV2, respectively.\n".format(argv[0]))

    exit(1)

stdev1 = float(argv[1])
stdev2 = float(argv[2])

# Read generation t and parameter from stdin
stderr.write("Enter t\n")
t = int(stdin.readline())

stderr.write("Enter beta and gamma\n")
beta, gamma = [ float(num) for num in stdin.readline().split() ]

# Print some information
stderr.write("Generation: {}\n".format(t))

# Perturb parameter
beta_perturbed = normal(loc=beta, scale=stdev1)
gamma_perturbed = normal(loc=gamma, scale=stdev2)

# Print perturbed parameter
stdout.write("{} {}\n".format(beta_perturbed, gamma_perturbed))
