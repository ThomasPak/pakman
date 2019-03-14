#!/usr/bin/env python2
import matplotlib.pyplot as plt
import numpy as np
import csv
import sys

# Process arguments
if not (len(sys.argv) == 3):
    print("Usage: {} SCALING_CSV_FILE SCALING_OUTPUT_PLOT".format(sys.argv[0]))
    exit(1)

# Read data
num_procs = []
compute_time = []
with open(sys.argv[1], 'r') as csvfile:

    data_reader = csv.reader(csvfile, delimiter=',')

    # Skip header
    next(data_reader)

    # Process line by line
    for row in data_reader:
        num_procs.append(float(row[0]))
        compute_time.append(float(row[1]))

num_procs = np.array(num_procs)
compute_time = np.array(compute_time)

# Estimate slope
num_procs_log = np.log(num_procs)
compute_time_log = np.log(compute_time)

A = np.vstack([num_procs_log, np.ones(len(num_procs_log))]).T
slope, offset = np.linalg.lstsq(A, compute_time_log, rcond=-1)[0]

print("Estimated slope: {}".format(slope))

# Plot data and fitted line
plt.figure()

plt.loglog(num_procs, compute_time, 'ko', label="data")
plt.loglog([num_procs[0], num_procs[-1]],
    [np.exp(offset + num_procs_log[0]*slope),
        np.exp(offset + num_procs_log[-1]*slope)],
    'b-', label="estimated slope: {}".format(slope))

# Formatting
plt.xticks(num_procs, np.array(num_procs, dtype=int))
plt.ylim(top=10.0**np.ceil(np.log10(compute_time[0])))
plt.ylim(bottom=10.0**np.floor(np.log10(compute_time[-1])))

plt.legend()
plt.xlabel("Number of processes used")
plt.ylabel("Computation time (s)")

# Save figure
plt.savefig(sys.argv[2])
