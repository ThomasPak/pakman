#!/bin/bash
set -euo pipefail

# Create data file
echo 5 > biased-coin-flip.data

# Create input file
./biased-coin-flip.py generate-input-text 20 0 biased-coin-flip.data \
    > biased-coin-flip.rej

# Run pakman rejection algorithm
mpirun -n 4 pakman rejection biased-coin-flip.rej 1000 > biased-coin-flip.out

# Remove N
cat biased-coin-flip.out | awk -F, '{ print $1 }' > biased-coin-flip.tmp
mv biased-coin-flip.tmp biased-coin-flip.out

# Create histogram
./create-histogram.py 5 20 --epsilon 0 --inputfile biased-coin-flip.out \
    --bboxtight --outputfile histogram.png

# Remove files
rm biased-coin-flip.data biased-coin-flip.rej
