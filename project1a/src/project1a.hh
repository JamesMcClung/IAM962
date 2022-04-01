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

////////////////////////////////////////////////////////////////////////
// constants as variables for actual use in the code

static const double nu = NU;
static const int nx = NX;
static const int nt = NT;
static const double min_x = MIN_X;
static const double max_x = MAX_X;
static const double len_x = max_x - min_x;
static const double min_x_bc = MIN_X_BC;
static const double max_x_bc = MAX_X_BC;

using real = double;