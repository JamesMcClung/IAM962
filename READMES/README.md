# IAM 962: Numerical PDES Project Submissions
_James McClung_

## Overview

I use C++ to write the simulations, bash scripts to automate tasks, python scripts to process the results of the simulations, and Markdown to write the reports.

There are 3 projects, each with up to 3 subprojects. All results are in Markdown in the subdirectories of READMES.

## Scripts

All python and bash scripts are assumed to be executed from their immediate _parent_ directory. For example, running `./build.sh` will not work, but `./scripts/build.sh` should (from the appropriate directories).

## Python Dependencies

The root directory of this project contains `python/env.yml`. It contains a list of all the python dependencies used throughout this project. If you have Conda, the scripts `scripts/python_create_env.sh` and `scripts/python_actiave_env.sh` will create and activate a Python environment with those dependencies, respectively.

## Linalg

I wrote my own linear algebra library for fun and used it in this project. It is included as a git submodule at `external/linalg/`. It isn't as well commented, but it should not be necessary to look in there.

## Compiling

I use CMake to make the makefiles for the source code. Nothing is unusual about that.

What might be unusual is that I made it so that I can pass arguments to the makefile, and from there define parameters in the source code at _compile time_. For example, `make project1a BUILDPARAMS="dt=.01 nx=256"` builds a simulation with `DT=.01` and `NX=256`. Various `build.sh` scripts wrap that, so `./scripts/build.sh dt=.01 nx=256` would be equivalent to the above example if you are in the `project1a/` directory.

This is done by a special target that passes `BUILDPARAMS` to `python/generate_buildparams.py`, which generates a header file defining the given parameters. This header is then included by the source code, and anything not defined in it is defined by a `defaultparams.h`. The latter can of course be edited directly to produce the same results, but that's annoying, especially with git. Parameter files (similar to Fortran's namelists) would have been a less unusual approach, but with the huge disadvantage of being runtime-only. In particular, this would requre that all matrices be dynamically allocated.