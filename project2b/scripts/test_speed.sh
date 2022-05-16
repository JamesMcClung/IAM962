#!/bin/bash

# this probably isn't portable

echo nx nruns meantime > results.csv

NRUNS=10

for NX in 64 128 256 512 1024 2048
do
    ./scripts/build.sh nx=$NX
    TIME=0
    for _ in $(seq 1 $NRUNS)
    do
        # bc does floating point math, and scale=3 sets how many decimal places to output
        # sed extracts the wall time ("real") from the time command
        TIME=$(echo "scale=3; $TIME+$(/usr/bin/time ./scripts/run.sh 2>&1 | sed -r 's/.*([0-9]+\.[0-9]+).*real.*/\1/')" | bc)
    done
    TIME=$(echo "scale=3; $TIME/$NRUNS" | bc)
    echo $NX $NRUNS $TIME >> results.csv
done