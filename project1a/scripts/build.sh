#! /bin/bash

# Example usage: ./scripts/build.sh NX=256 DT=.1

args=$@
cd ../build; make project1a BUILDPARAMS="${args[@]}"