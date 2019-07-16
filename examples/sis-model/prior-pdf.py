from sys import argv, stdin, stdout, stderr

# Process arguments
if len(argv) != 5:
    stderr.write("Usage: {} BETA_LOW BETA_HIGH GAMMA_LOW GAMMA_HIGH\n"
            "Return probability density of prior distribution with\n"
            "uniformly distributed beta with lower bound BETA_LOW and upper BETA_HIGH,\n"
            "and uniformly distributed gamma with lower bound GAMMA_LOW and upper GAMMA_HIGH,\n"
            "for given parameter\n".format(argv[0]))

    exit(1)

beta_low = float(argv[1])
beta_high = float(argv[2])
gamma_low = float(argv[3])
gamma_high = float(argv[4])

# Read parameter from stdin
stderr.write("Enter beta and gamma\n")
beta, gamma = [ float(num) for num in stdin.readline().split() ]

# Check if beta, gamma is within bounds
if beta_low <= beta and beta <= beta_high \
        and gamma_low <= gamma and gamma <= gamma_high:
            density = 1.0 / (beta_high - beta_low) / (gamma_high - gamma_low)
            stdout.write("{}\n".format(density))
else:
    stdout.write("0\n")
