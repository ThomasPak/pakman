from numpy.random import uniform
from numpy import log, sqrt
from sys import argv, stdin, stdout, stderr

# Process arguments
if len(argv) < 6:
    stderr.write("Usage: {} S0 I0 TEND NOBS DATAFILE [TRACEFILE]\n"
            "Read S and I counts from DATAFILE, "
            "epsilon and parameters beta and gamma from stdin,\n"
            "run simulation and output whether "
            "parameter is accepted or rejected\n"
            "If TRACEFILE is given, save the t and S series to TRACEFILE\n".format(argv[0]))

    exit(1)

S0 = int(argv[1])
I0 = int(argv[2])
tend = float(argv[3])
Nobs = int(argv[4])

datafile = argv[5]
if len(argv) >= 7:
    tracefile = argv[6]
else:
    tracefile = None

# Read epsilon and q from stdin
stderr.write("Enter epsilon\n");
epsilon = int(stdin.readline())

stderr.write("Enter beta and gamma\n");
beta, gamma = [ float(num) for num in stdin.readline().split() ]

# Read datafile into array of S counts
S_obs = []
with open(datafile, 'r') as dfile:
    for line in dfile:
        S_obs.append(int(line))

def run_SIS_simulation(beta, gamma, S0, I0, tend, Nobs, tracefile=None):
    '''
    Run Gillespie algorithm for SIS system and return time series of S.

    Parameters
    ----------
    beta : scalar
        Contact rate parameter.
    gamma : scalar
        Recovery rate parameter.
    S0 : int
        Initial number of susceptible people.
    I0 : int
        Initial number of infected people.
    tend : scalar
        End of simulation time.
    Nobs : int
        Number of observations to output.
    tracefile : str, optional
        Name of file to save S trace to.

    Returns
    -------
    S_sim : numpy array of int values
        Array containing simulated S counts at times
        t = 0, t = dt, t = 2 * dt, ... t = tend,
        where dt = tend / Nobs.

    Notes
    -----
    A thorough explanation of the SIS model can be found at
    https://github.com/ThomasPak/pakman/wiki/Example:-SIS-model.
    '''

    # Initialize arrays
    t = [0.0]
    S = [S0]
    I = [I0]

    # Run Gillespie algorithm
    while t[-1] <= tend:

        # If I == 0, the total propensity will be zero and no more reactions
        # can occur
        if I[-1] == 0:
            break

        # Sample two uniformly distributed variables
        r1, r2 = uniform(size=2)

        # Compute propensities
        a1 = beta * S[-1] * I[-1]
        a2 = gamma * I[-1]
        a0 = a1 + a2

        # Compute waiting time
        twait = 1.0 / a0 * log(1.0 / r1)
        t.append(t[-1] + twait)

        # Choose reaction
        if r2 * a0 < a1:
            # S + I --> 2I
            S.append(S[-1] - 1)
            I.append(I[-1] + 1)
        else:
            S.append(S[-1] + 1)
            I.append(I[-1] - 1)

    # Extract S at desired time points
    S_sim = []
    dt = tend / Nobs
    tnow = dt

    S_idx = 0
    t_idx = 0

    while len(S_sim) < Nobs:
        # If end of array has been reached, break out of loop
        if t_idx + 1 == len(t):
            break

        # Check if current time point is between t[t_idx] and t[t_idx + 1]
        if t[t_idx] < tnow and tnow <= t[t_idx + 1]:
            S_sim.append(S[S_idx])
            tnow += dt
        else:
            t_idx += 1
            S_idx += 1

    # Fill in remaining entries
    S_sim += [ S[-1] for i in range(Nobs - len(S_sim)) ]

    # If tracefile is not None, save t and S to tracefile
    if not tracefile is None:
        with open(tracefile, 'w') as tfile:
            for t_, S_ in zip(t, S):
                tfile.write("{} {}\n".format(t_, S_))

    return S_sim

def distance(S_obs, S_sim):
    '''
    Return square sum of squared differences between observed and simulated S
    counts.

    Parameters
    ----------
    S_obs : array_like
        Time series of observed S counts.
    S_sim : array_like
        Time series of simulated S counts.

    Returns
    -------
    scalar
        Sum of squared differences.
    '''
    return sqrt(sum((obs - sim)**2 for obs, sim in zip(S_obs, S_sim)))

# Run simulation
S_sim = run_SIS_simulation(beta, gamma, S0, I0, tend, Nobs, tracefile)

# Compute distance
dist = distance(S_obs, S_sim)

# Print some information
stderr.write("Observed S: {}\n".format(S_obs))
stderr.write("Simulated S: {}\n".format(S_sim))
stderr.write("L2 difference: {}\n".format(dist))

# If absolute difference is less or equal than epsilon, accept, else reject.
if dist <= epsilon:
    stdout.write("accept\n")
else:
    stdout.write("reject\n")
