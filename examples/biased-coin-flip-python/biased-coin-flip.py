#!/usr/bin/env python2
from numpy.random import binomial, uniform
from sys import argv, stdin, stdout, stderr

def run_biased_coin_trial(n, p):
    return binomial(n, p)

def sample_prior(N):
    return "{} {}".format(uniform(), N)

def distance_function(m_sim, m_data):
    return abs(m_sim - m_data)

def run_simulation(e, n, p, m_data):
# Run trial
    m_sim = run_biased_coin_trial(n, p)

# Calculate distance to data
    dist = distance_function(m_sim, m_data)

    stderr.write("m_sim, m_data, dist, e: {}, {}, {}, {}\n".format(
        m_sim, m_data, dist, e))

# Return true if distance is less than epsilon
    return dist <= e

def run_simulator(data_filename):
# Read epsilon
    e = int(stdin.readline())

# Read parameter
    parameter_list = stdin.readline().split()

    p = float(parameter_list[0])
    n = int(parameter_list[1])

# Read data from file
    with open(data_filename, 'r') as datafile:
        m_data = int(datafile.read())

# Run simulation; write 1 if accepted, else 0
    accepted = run_simulation(e, n, p, m_data)
    if accepted:
        stdout.write("1\n")
    else:
        stdout.write("0\n")

def generate_input_text(N, e, data_filename):
    input_text = ""

    input_text += "# Epsilon\n"
    input_text += "{}\n".format(e)
    input_text += "\n"

    input_text += "# Simulator\n"
    input_text += "{} run-simulator {}\n".format(argv[0], data_filename)
    input_text += "\n"

    input_text += "# Parameter names\n"
    input_text += "p,N\n"
    input_text += "\n"

    input_text += "# Prior sampler\n"
    input_text += "{} sample-prior {}\n".format(argv[0], N)

    return input_text

def print_help_message():

    help_msg = """Usage: {} sample-prior | run-simulator | generate-input-text
sample-prior N
    sample from uniform prior with N trials

run-simulator DATAFILE
    run simulator reading data from DATAFILE

generate-input-text N EPSILON DATAFILE
    generate input text to pakman with N trials, tolerance EPSILON
    and data from DATAFILE
""".format(argv[0])

    stdout.write(help_msg)

if __name__ == "__main__":

# Check that at least one argument is given
    if not len(argv) >= 2:
        print_help_message()
        exit(2)

# Switch on argv[1]
    if argv[1] == "sample-prior":
        if not len(argv) == 3:
            print_help_message()
            exit(2)

        print(sample_prior(int(argv[2])))

    elif argv[1] == "run-simulator":
        if not len(argv) == 3:
            print_help_message()
            exit(2)

        run_simulator(argv[2])

    elif argv[1] == "generate-input-text":
        if not len(argv) == 5:
            print_help_message()
            exit(2)

        stdout.write(generate_input_text(int(argv[2]), int(argv[3]), argv[4]))
    else:
        print_help_message()
        exit(2)

    exit(0)

## Tests ##
# run_biased_coin_trial
    n = 20
    p = 0.25

    print("n, p: {}, {}".format(n, p))
    print("run_biased_coin_trial result: {}".format(run_biased_coin_trial(n, p)))

# sample_prior
    print("sample_prior result: {}".format(sample_prior(n)))

# distance_function
    m_sim = 3
    m_data = 5

    print("m_sim, m_data: {}, {}".format(m_sim, m_data))
    print("distance_function result: {}".format(distance_function(m_sim, m_data)))

# run_simulation
    e = 0

    print("e: {}".format(e))
    print("run_simulation result: {}".format(run_simulation(e, n, p, m_data)))

# run_simulator
    data_filename = "biased-coin-flip.data"
    run_simulator(data_filename)

# generate_input_text
    print("generate_input_text result: {}".format(generate_input_text(n, e, data_filename)))

# print_help_message
    print("Calling print_help_message()...")
    print_help_message()
