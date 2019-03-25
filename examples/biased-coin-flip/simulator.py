from numpy.random import uniform
from sys import argv, stdin, stdout, stderr

# Process arguments
if len(argv) != 2:
    stderr.write("Usage: {} DATAFILE\n"
            "Read number of heads from DATAFILE, "
            "epsilon and parameter from stdin,\n"
            "run simulation and output whether "
            "parameter is accepted or rejected\n".format(argv[0]))

    exit(1)

datafile = argv[1]

# Read epsilon and q from stdin
stderr.write("Enter epsilon\n");
epsilon = int(stdin.readline())

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

# If absolute difference is less or equal than epsilon, accept, else reject.
if abs(sim_num_heads - obs_num_heads) <= epsilon:
    stdout.write("accept\n")
else:
    stdout.write("reject\n")
