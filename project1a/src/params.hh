#pragma once

////////////////////////////////////////////////////////////////////////
// constants as macros so they can eventually be defined during
//   compilation

#define NU .01
#define NX 128;
#define NT 1024;
#define MIN_X 0;
#define MAX_X 1;
#define MIN_X_BC 0;
#define MAX_X_BC 0;

#define WRITE_EVERY 8;
#define OUT_FILE "out.csv";

////////////////////////////////////////////////////////////////////////
// constants as variables for actual use in the code

using real = double;  // type for values of u

static const real nu = NU;                // diffusivity constant: u_t = nu * u_xx
static const int nx = NX;                 // number of spatial gridpoints
static const int nt = NT;                 // number of temporal gridpoints (time steps to run for)
static const real min_x = MIN_X;          // lower bound for x domain
static const real max_x = MAX_X;          // upper bound for x domain
static const real len_x = max_x - min_x;  // length of x domain
static const real dx = len_x / nx;        // distance between spatial gridpoints
static const real dt = dx * dx / nu;      // distance between temporal gridpoints
static const real min_x_bc = MIN_X_BC;    // boundary condition for u(min_x)
static const real max_x_bc = MAX_X_BC;    // boundary condition for u(max_x)

static const int write_every = WRITE_EVERY;  // write u every <this many> time steps
static const auto out_file = OUT_FILE;       // path to output