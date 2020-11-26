from numpy.random import uniform, seed
from sys import argv, stdout, stderr

# Process arguments
if len(argv) < 3:
    stderr.write("Usage: {} Q_LOW Q_HIGH [SEED_FILE]\n"
            "Sample from a uniform distribution with "
            "lower bound Q_LOW and upper bound Q_HIGH\n"
            "If given SEED_FILE, read seed from file".format(argv[0]));

    exit(1)

q_low = float(argv[1])
q_high = float(argv[2])

if len(argv) == 4:

    # Open seed file
    try:
        seed_file = open(argv[3])
        current_seed = int(seed_file.read())
        seed_file.close()
    except:
        stderr.write("Error: could not read seed from file {}\n".format(argv[3]))
        exit(1)

    # Seed random number generator
    seed(current_seed)

    # Increment seed in seed file
    seed_file = open(argv[3], 'w')
    seed_file.write(str(current_seed + 1))
    seed_file.close()

# Sample from uniform distribution
q_sampled = uniform(low=q_low, high=q_high)

# Print sampled parameter
stdout.write("{}\n".format(q_sampled))
