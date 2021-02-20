from numpy.random import uniform, seed
from sys import argv, stdin, stdout, stderr

# Process arguments
if len(argv) < 2:
    stderr.write("Usage: {} DATAFILE [SEED_FILE]\n"
            "Read number of heads from DATAFILE, "
            "and parameter from stdin,\n"
            "run simulation and output the distance "
            "between simulated and observed data\n"
            "If given SEED_FILE, read seed from file".format(argv[0]))

    exit(1)

datafile = argv[1]

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

# Read q from stdin
stderr.write("Enter q\n");
q = float(stdin.readline())

# Read datafile into number of heads
with open(datafile, 'r') as dfile:
    obs_num_heads = int(dfile.read())

# Run simulation
sim_num_heads = 0
for i in range(20):
    if uniform() < q:
        sim_num_heads += 1

# Return absolute distance between simulated and observed number of heads
stdout.write("{}\n".format(abs(sim_num_heads - obs_num_heads)))
