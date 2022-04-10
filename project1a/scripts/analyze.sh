#!/bin/bash

echo nu cfl error > results.csv

for NU in .0001 #.01 1
do
    for CFL in $(seq .1 .05 3)
    do
        ./scripts/build_and_run.sh nu=$NU cfl=$CFL
        echo $NU $CFL $(python3 python/find_damping_rate.py --bare out.csv) >> results.csv
        # pause is necessary or else script can build and run while python
        #   is still analyzing data, apparently
        sleep .5
    done
done