#pragma once
#include "buildparams.h"
#include "config.hh"

using real = double;  // type for values of u

static constexpr real nu = P1A_NU;                          // diffusivity constant: u_t = nu * u_xx
static constexpr int nx = P1A_NX;                           // number of spatial gridpoints
static constexpr int nt = P1A_NT;                           // time steps to run for
static constexpr real min_x = P1A_MIN_X;                    // lower bound for x domain
static constexpr real max_x = P1A_MAX_X;                    // upper bound for x domain
static constexpr real len_x = max_x - min_x;                // length of x domain
static constexpr real dx = len_x / nx;                      // distance between spatial gridpoints
#ifdef P1A_DT
static constexpr real dt = P1A_DT;  // time between time steps
#else
#define USE_DT_OPT
static constexpr real dt = dx * dx / nu;  // a convenient choice for dt
#endif
static constexpr real min_x_bc = P1A_MIN_X_BC;              // boundary condition for u(min_x)
static constexpr real max_x_bc = P1A_MAX_X_BC;              // boundary condition for u(max_x)

static constexpr int write_every = P1A_WRITE_EVERY;  // write u every <this many> time steps
static constexpr auto out_file = P1A_OUT_FILE;       // path to output