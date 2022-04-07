#! /bin/bash

# Example usage: ./scripts/build.sh NX=256 DT=.1

args=$@
cd ../build
make project1b BUILDPARAMS="${args[@]}"
cd ../project1b
../build/project1b/project1b