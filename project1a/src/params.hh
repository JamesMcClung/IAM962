#pragma once

////////////////////////////////////////////////////////////////////////
// constants as macros so they can eventually be defined during
//   compilation

#define NU .01       // diffusivity constant
#define NX 128;      // number of grid points
#define NT 1024;     // number of time steps to run for
#define MIN_X 0;     // lower bound for x domain
#define MAX_X 1;     // upper bound for x domain
#define MIN_X_BC 0;  // boundary condition for u(min_x)
#define MAX_X_BC 0;  // boundary condition for u(max_x)

#define WRITE_EVERY 8;        // write u every <this many> time steps
#define OUT_FILE "out.csv";   // path to output

////////////////////////////////////////////////////////////////////////
// constants as variables for actual use in the code

using real = double;

static const real nu = NU;
static const int nx = NX;
static const int nt = NT;
static const real min_x = MIN_X;
static const real max_x = MAX_X;
static const real len_x = max_x - min_x;
static const real dx = len_x / nx;
static const real dt = dx * dx / nu;
static const real min_x_bc = MIN_X_BC;
static const real max_x_bc = MAX_X_BC;

static const int write_every = WRITE_EVERY;
static const auto out_file = OUT_FILE;