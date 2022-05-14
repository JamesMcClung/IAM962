#pragma once

#include <string_view>

#include "buildparams.h"
#include "defaultparams.hh"  // default params must be included after build params
#include "linalg/complex.hh"

using real = double;                    // precision
using complex = linalg::Complex<real>;  // type for values of u and uhat

static constexpr real a = A;                  // coefficient of kinetic term
static constexpr real b = B;                  // coefficient of nonlinear term
static constexpr int nx = NX;                 // number of spatial gridpoints
static constexpr int nt = NT;                 // time steps to run for
static constexpr real min_x = MIN_X;          // lower bound for x domain
static constexpr real max_x = MAX_X;          // upper bound for x domain
static constexpr real len_x = max_x - min_x;  // length of x domain
static constexpr real dx = len_x / nx;        // distance between spatial gridpoints
static constexpr real dt = DT;                // time between time steps

static constexpr std::string_view out_mode(OUT_MODE);  // whether to write real, imaginary, or mag2
static constexpr int write_every = WRITE_EVERY;        // write u every <this many> time steps
static constexpr auto out_file = OUT_FILE;             // path to output

#undef A
#undef B
#undef NX
#undef NT
#undef MIN_X
#undef MAX_X
#undef DT
#undef OUT_MODE
#undef WRITE_EVERY
#undef OUT_FILE