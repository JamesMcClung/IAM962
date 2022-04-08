#! /bin/bash

# Example usage: ./scripts/build.sh NX=256 DT=.1

args=$@
cd ../build
make project1c BUILDPARAMS="${args[@]}"
cd ../project1c
../build/project1c/project1c