from numpy.random import uniform
from sys import argv, stdin, stdout, stderr

# Process arguments
if len(argv) != 2:
    stderr.write("Usage: {} DATAFILE\n"
            "Read number of heads from DATAFILE, "
            "and parameter from stdin,\n"
            "run simulation and output the distance "
            "between simulated and observed data\n".format(argv[0]))

    exit(1)

datafile = argv[1]

# Read q from stdin
stderr.write("Enter q\n");
q = float(stdin.readline())

# Run simulation
sim_num_heads = 0
for i in range(20):
    if uniform() < q:
        sim_num_heads += 1

# Read datafile for observed number of heads
with open(datafile, 'r') as dfile:
    obs_num_heads = int(dfile.read())

# Print some information
stderr.write("Observed heads: {}\n".format(obs_num_heads))
stderr.write("Simulated heads: {}\n".format(sim_num_heads))

# Return absolute distance between simulated and observed number of heads
stdout.write("{}\n".format(abs(sim_num_heads - obs_num_heads)))
