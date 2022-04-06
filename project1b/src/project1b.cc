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
//   Adams-Bashforth (AB) method in time and central difference in space.

// AB methods solve u_t = F(u) explicitly. For the wave equation,
//   F(u) = -c * d/dx[u]. We replace the d/dx operator by the
//   central difference operator, Dx[u_j] = (u_{j+1} - u_{j-1}) / (2dx).

// Notation:
//   u^{n-k}_j     -> uk      (k = 0, 1, ...)
//   u^{n-k}_{j-1} -> uk_     (k = 0, 1, ...)
//   u^{n-k}_{j+1} -> uk^     (k = 0, 1, ...)
//   u^{n+1}_j     -> v
//   b := dt * c / (2dx)
//   duk := -b * (uk^ - uk_)  (k = 0, 1, ...)

// Central difference in space:
//   => F(uk) = -c / (2dx) * (uk^ - uk_)
//            = duk / dt

// AB1 with central difference in space can then be written as
//   v = u0 + dt * F(u0)
//     = u0 + du0

// AB2 with central difference in space as
//   v = u0 + dt/2 * (3F(u0) - F(u1))
//     = u0 + 1/2 * (3du0 - du1)

// AB3 with central difference in space as
//   v = u0 + dt/12 * (23F(u0) - 16F(u1) + 5F(u2))
//     = u0 + 1/12 * (23du0 - 16du1 + 5du2)

// AB4 with central difference in space as
//   v = u0 + dt/24 * (55F(u0) - 59F(u1) + 37F(u2) - 9F(u3))
//     = u0 + 1/24 * (55du0 - 59du1 + 37du2 - 9du3)

////////////////////////////////////////////////////////////////////////
//                              PROGRAM                               //
////////////////////////////////////////////////////////////////////////

using u_type = linalg::FullMatrix<nx, 1, real>;

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