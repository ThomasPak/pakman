#!/usr/bin/env python2
import numpy as np
import matplotlib.pyplot as plt
import sys

from argparse import ArgumentParser

# Process arguments
ap = ArgumentParser(description='Plot histogram and analytical likelihood of binomial model')
ap.add_argument('k', type=int, help='number of successes')
ap.add_argument('n', type=int, help='nuber of trials')
ap.add_argument('-e', '--epsilon', type=str, help='epsilon used to generate data',
                default=None)
ap.add_argument('-o', '--outputfile', help='file to save figure to',
                default = 'binomial_histogram.png')
ap.add_argument('-i', '--inputfile', help='file to read data from (default: STDIN)',
                default = sys.stdin)
ap.add_argument('-H', '--plotheight', type=float, help='upper bound on plotting height',
            default=None)
ap.add_argument('-b', '--bboxtight', action='store_true', help='make bbox around plot tight')
args = ap.parse_args()

k = args.k
n = args.n
e = args.epsilon
plotheight = args.plotheight
bboxtight = args.bboxtight
outputfile = args.outputfile
inputfile = args.inputfile

# Process header
if type(inputfile) == str:
    inputfile = open(inputfile, 'r')
header = inputfile.next()

# Process data
data = []

for line in inputfile:
    data.append(float(line))

data = np.array(data)

print("data: {}".format(data))

# Analytical log-likelihood function
logC = (  ( np.sum(np.log(np.arange(1, n + 1))) -
             np.sum(np.log(np.arange(1, k + 1))) -
             np.sum(np.log(np.arange(1, n - k + 1))) ) +
         np.log(n + 1) )
loglikelihood = lambda p: logC + k * np.log(p) + (n - k) * np.log(1 - p)

# Plot histogram
bins = 35
p = np.linspace(0.001, 1 - 0.001, 1001)
fig = plt.figure()

# Plot computed histogram
plt.hist(data, bins=bins, normed=True, label="Computed", edgecolor='black')
plt.plot(p, np.exp(loglikelihood(p)), label="Analytical")

titlestr = "Posterior with N = {}".format(len(data))
if e != None:
    titlestr += ", $\epsilon$ = {}".format(e)

plt.title(titlestr)
plt.xlabel("q")
plt.ylabel("Posterior distribution")
plt.legend()

# Set height
if plotheight != None:
    ylim = plt.ylim()
    plt.ylim([ylim[0], plotheight])

# Save histogram
plt.savefig(outputfile, bbox_inches='tight' if bboxtight else None)

# Close input file
inputfile.close()
