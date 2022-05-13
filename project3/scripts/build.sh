#! /bin/bash

# Example usage: ./scripts/build.sh NX=256 DT=.1

args=$@
(cd ../build; make project3 BUILDPARAMS="${args[@]}")