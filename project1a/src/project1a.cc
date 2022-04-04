#include <cmath>
#include <iostream>

#include "linalg/bandedmatrix.hh"
#include "linalg/fullmatrix.hh"
#include "linalg/matrix_io.hh"
#include "linalg/thomas.hh"
#include "params.hh"

////////////////////////////////////////////////////////////////////////
//                            DESCRIPTION                             //
////////////////////////////////////////////////////////////////////////

// Author: James McClung
// Month: April 2022

// This program solves the heat equation, u_t = nu * u_xx, using the
//   Crank-Nicolson (C-N) method with central difference in space.

// Notation:
//   u^n_j         -> u
//   u^n_{j-1}     -> u_
//   u^n_{j+1}     -> u^
//   u^{n+1}_j     -> v
//   u^{n+1}_{j-1} -> v_
//   u^{n+1}_{j+1} -> v^
//   b := 2 * dx^2 / (dt * nu)

// C-N with central difference in space can then be written as
//   b(v - u) = v^ - 2v + v_ + u^ - 2u + u_
//   => -v_ + (b+2)v - v^ = u_ + (b-2)u + u^,
//   which is a tridiagonal system.

// Note that when dt = dx^2 / nu, we get b=2, and the scheme reduces
//   further to
//   => -v_ + 4v - v^ = u_ + u^.
// This case is handled separately to optimize out the call to u.

////////////////////////////////////////////////////////////////////////
//                              PROGRAM                               //
////////////////////////////////////////////////////////////////////////

using u_type = linalg::FullMatrix<nx, 1, real>;
static linalg::BandedMatrix<nx, 1, 1, real> coefs_CN(-1);

void set_to_initial_conditions(u_type &u) {
    for (int i = 0; i < nx; i++) {
        real x = i * len_x / nx + min_x;
        u(i, 0) = std::sin(4 * M_PI * x);
    }
}

void initialize_coefs_CN() {
    for (int r = 0; r < nx; r++) {
        // off-diagonals are already -1, so just set main diagonal
#ifdef USE_DT_OPT
        coefs_CN(r, r) = 4;
#else
        coefs_CN(r, r) = 2 + 2 * dx * dx / (dt * nu);
#endif
    }
}

void solve_for_next_u(const u_type &u, u_type &next_u) {
#ifdef USE_DT_OPT
    for (int r = 1; r < nx - 1; r++) {
        next_u(r, 0) = u(r - 1, 0) + u(r + 1, 0);
    }
    next_u(0, 0) = min_x_bc + u(1, 0);
    next_u(nx - 1, 0) = u(nx - 2, 0) + max_x_bc;
#else
    for (int r = 1; r < nx - 1; r++) {
        next_u(r, 0) = u(r - 1, 0) + (2 * dx * dx / (dt * nu) - 2) * u(r, 0) + u(r + 1, 0);
    }
    next_u(0, 0) = min_x_bc + (2 * dx * dx / (dt * nu) - 2) * u(0, 0) + u(1, 0);
    next_u(nx - 1, 0) = u(nx - 2, 0) + (2 * dx * dx / (dt * nu) - 2) * u(nx - 1, 0) + max_x_bc;
#endif

    thomas_in_place(coefs_CN, next_u);
}

void write_params() {
    linalg::FullMatrix params_mat({{nu, real(nx), real(nt), min_x, max_x, dx, dt, min_x_bc, max_x_bc, real(write_every)}});
    saveMatrix(out_file, params_mat, false);
}

void write_u(const u_type &u) {
    saveMatrix_transpose(out_file, u, true);
}

int main() {
    std::unique_ptr<u_type> this_u(new u_type);
    std::unique_ptr<u_type> next_u(new u_type);

    set_to_initial_conditions(*this_u);
    initialize_coefs_CN();

    write_params();

    for (int i = 0; i < nt; i++) {
        if (i % write_every == 0) {
            write_u(*this_u);
        }

        solve_for_next_u(*this_u, *next_u);
        this_u.swap(next_u);
    }

    write_u(*this_u);

    return 0;
}