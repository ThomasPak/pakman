from numpy import pi, sqrt, exp
from sys import argv, stdin, stdout, stderr

# Process arguments
if len(argv) != 3:
    stderr.write("Usage: {} STDEV1 STDEV2\n"
            "Return probability densities of joint normal distributions centered \n"
            "on given parameters for the perturbed parameter \n"
            "and with standard deviations STDEV1 and STDEV2\n".format(argv[0]))

    exit(1)

stdev1 = float(argv[1])
stdev2 = float(argv[2])

# Read generation t and perturbed parameter from stdin
stderr.write("Enter t\n")
t = int(stdin.readline())

stderr.write("Enter beta_perturbed and gamma_perturbed\n")
beta_perturbed, gamma_perturbed = \
        [ float(num) for num in stdin.readline().split() ]

# Define normal pdf
normal_pdf = lambda mu, stdev, x: 1.0 / sqrt(2 * pi * stdev**2) * \
        exp( - (x - mu)**2 / (2.0 * stdev**2))

# Read parameters from stdin
stderr.write("Enter parameters\n")
beta_array = []
gamma_array = []
for line in stdin:
    beta, gamma = [ float(num) for num in line.split() ]
    beta_array.append(beta)
    gamma_array.append(gamma)

# Return normal_pdf for every parameter
for beta, gamma in zip(beta_array, gamma_array):
    pdf1 = normal_pdf(beta, stdev1, beta_perturbed)
    pdf2 = normal_pdf(gamma, stdev2, gamma_perturbed)
    pdf = pdf1 * pdf2
    stdout.write("{}\n".format(pdf))
