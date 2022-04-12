#!/bin/bash

echo ab cfl dr > results.csv

for AB in 1 2 3 4 5
do
    for CFL in $(seq .05 .05 .7)
    do
        ./scripts/build_and_run.sh which_ab=$AB cfl=$CFL
        echo $AB $CFL $(python3 python/find_stability.py --bare out.csv) >> results.csv
        # pause is necessary or else script can build and run while python
        #   is still analyzing data, apparently
        sleep .5
    done
done