from sys import argv, stdin, stdout, stderr

# Process arguments
if len(argv) != 3:
    stderr.write("Usage: {} Q_LOW Q_HIGH\n"
            "Return probability density of uniform distribution with \n"
            "lower bound Q_LOW and upper bound Q_HIGH "
            "for given parameter\n".format(argv[0]))

    exit(1)

q_low = float(argv[1])
q_high = float(argv[2])

# Read parameter from stdin
stderr.write("Enter q\n")
q = float(stdin.readline())

# Check if q is within bounds
if q_low <= q and q <= q_high:
    density = 1.0 / (q_high - q_low)
    stdout.write("{}\n".format(density))
else:
    stdout.write("0\n")
