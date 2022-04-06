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

// This program solves the wave equation, u_t + c * u_x = 0, using an
//   Adams-Bashforth method in time and central difference in space.

////////////////////////////////////////////////////////////////////////
//                              PROGRAM                               //
////////////////////////////////////////////////////////////////////////

using u_type = linalg::FullMatrix<nx, 1, real>;
static linalg::BandedMatrix<nx, 1, 1, real> coefs_CN(-1);

void set_to_initial_conditions(u_type &u) {
    for (int i = 0; i < nx; i++) {
        real x = i * len_x / nx + min_x;
        u(i, 0) = std::sin(2 * M_PI * x);
    }
}

void solve_for_next_u(const u_type &u, u_type &next_u) {
    // TODO
}

void write_params() {
    linalg::FullMatrix params_mat({{c, real(nx), real(nt), min_x, max_x, dx, dt, real(write_every)}});
    saveMatrix(out_file, params_mat, false);
}

void write_u(const u_type &u) {
    saveMatrix_transpose(out_file, u, true);
}

int main() {
    std::unique_ptr<u_type> this_u(new u_type);
    std::unique_ptr<u_type> next_u(new u_type);

    set_to_initial_conditions(*this_u);

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