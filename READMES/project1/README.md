# Project 1

_by James McClung_

## Overview

I use C++ to write the simulations, bash scripts to automate tasks, and python scripts to process the results of the simulations. 

I broke this project down into 3 orthogonal parts:

- **Project 1a:** Heat equation via Crank-Nicolson
- **Project 1b:** Wave equation via Adams-Bashforth family
- **Project 1c:** Burger's equation via Adams-Bashforth family

See the respective markdown files for results.

The Python and C++ files themselves are also documented via comments.

## Scripts

All python and bash scripts are assumed to be executed from their immediate _parent_ directory. For example, running `./build.sh` will not work, but `./scripts/build.sh` should.

## Python Dependencies

The root directory of this project contains `python/env.yml`. It contains a list of all the python dependencies used throughout this project. If you have Conda, the scripts `scripts/python_create_env.sh` and `scripts/python_actiave_env.sh` will create and activate a Python environment with those dependencies, respectively. 

## Linalg

I wrote my own linear algebra library for fun and used it in this project. It is included as a git submodule at `external/linalg/`. It isn't as well commented, but it should not be necessary to look in there.

## Compiling

I use CMake to make the makefiles for the source code. Nothing is unusual about that.

What might be unusual is that I made it so that I can pass arguments to the makefile to pass definitions to the source code. For example, `make project1a BUILDPARAMS="dt=.01 nx=256"` builds a simulation with `DT=.01` and `NX=256`. Various `build.sh` scripts wrap that, so `./scripts/build.sh dt=.01 nx=256` would be equivalent to the above example if you are in the `project1a/` directory. This is done by a special target that passes `BUILDPARAMS` to `python/generate_buildparams.py`, which generates a header defining the given parameters. This header is then included by the source code, and anything not defined in it is defined by a `defaultparams.h`. The latter can of course be edited directly to produce the same results, but that's annoying, especially with git.