#!/bin/bash

# this probably isn't portable

echo nx sigma nruns meantime > results.csv

NRUNS=10

for NX in 64 128 256 512 1024
do
    ./scripts/build.sh nx=$NX
    TIME=0
    for _ in $(seq 1 $NRUNS)
    do
        TIME=$(echo "scale=3; $TIME+$(/usr/bin/time ./scripts/run.sh 2>&1 | sed -r 's/.*([0-9]+\.[0-9]+).*real.*/\1/')" | bc)
    done
    TIME=$(echo "scale=3; $TIME/$NRUNS" | bc)
    SIGMA=$(python3 python/plot_stability.py --bare out.csv)
    echo $NX $SIGMA $NRUNS $TIME >> results.csv
done