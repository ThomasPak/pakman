from numpy.random import normal, seed
from sys import argv, stdin, stdout, stderr

# Process arguments
if len(argv) < 2:
    stderr.write("Usage: {} STDEV [SEED_FILE]\n"
            "Perturb given parameter by drawing "
            "from normal distribution\n"
            "centered on the given parameter "
            "and with standard deviation STDEV\n"
            "If given SEED_FILE, read seed from file".format(argv[0]))

    exit(1)

stdev = float(argv[1])

if len(argv) == 3:

    # Open seed file
    try:
        seed_file = open(argv[2])
        current_seed = int(seed_file.read())
        seed_file.close()
    except:
        stderr.write("Error: could not read seed from file {}\n".format(argv[2]))
        exit(1)

    # Seed random number generator
    seed(current_seed)

    # Increment seed in seed file
    seed_file = open(argv[2], 'w')
    seed_file.write(str(current_seed + 1))
    seed_file.close()

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
