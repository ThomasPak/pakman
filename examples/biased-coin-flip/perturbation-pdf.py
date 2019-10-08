from numpy import pi, sqrt, exp
from sys import argv, stdin, stdout, stderr

# Process arguments
if len(argv) != 2:
    stderr.write("Usage: {} STDEV\n"
            "Return probability densities of normal distributions centered \n"
            "on given parameters for the perturbed parameter \n"
            "and with standard deviation STDEV\n".format(argv[0]))

    exit(1)

stdev = float(argv[1])

# Read generation t and perturbed parameter from stdin
stderr.write("Enter t\n")
t = int(stdin.readline())

stderr.write("Enter q_perturbed\n")
q_perturbed = float(stdin.readline())

# Define normal pdf
normal_pdf = lambda mu, x: 1.0 / sqrt(2 * pi * stdev**2) * \
        exp( - (x - mu)**2 / (2.0 * stdev**2))

# Read parameters from stdin
stderr.write("Enter parameters\n")
q_array = []
for line in stdin:
    q_array.append(float(line))

# Return normal_pdf for every parameter
for q in q_array:
    pdf = normal_pdf(q, q_perturbed)
    stdout.write("{}\n".format(pdf))
