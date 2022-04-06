#pragma once

#include "buildparams.h"
#include "defaultparams.h"  // default params must be included after build params

using real = double;  // type for values of u

static constexpr real c = C;                  // wave speed: u_t c * u_x
static constexpr int nx = NX;                 // number of spatial gridpoints
static constexpr int nt = NT;                 // time steps to run for
static constexpr real min_x = MIN_X;          // lower bound for x domain
static constexpr real max_x = MAX_X;          // upper bound for x domain
static constexpr real len_x = max_x - min_x;  // length of x domain
static constexpr real dx = len_x / nx;        // distance between spatial gridpoints
static constexpr real dt = DT;                // time between time steps

static constexpr int write_every = WRITE_EVERY;  // write u every <this many> time steps
static constexpr auto out_file = OUT_FILE;       // path to output