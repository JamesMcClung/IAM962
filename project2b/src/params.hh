#pragma once

#include "buildparams.h"
#include "defaultparams.hh"  // default params must be included after build params

using real = double;  // type for values of u

static constexpr real nu = NU;    // diffusion coefficient
static constexpr real c = C;      // coefficient of nonlinear term (for testing purposes)
static constexpr int nx = NX;     // number of spatial gridpoints
static constexpr int nt = NT;     // time steps to run for
static constexpr real cfl = CFL;  // Courant number, sort of
static constexpr real dt = DT;    // time between time steps

static constexpr int write_every = WRITE_EVERY;  // write u every <this many> time steps
static constexpr auto out_file = OUT_FILE;       // path to output