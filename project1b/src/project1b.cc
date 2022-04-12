#include <cmath>
#include <iostream>

#include "linalg/bandedmatrix.hh"
#include "linalg/fullmatrix.hh"
#include "linalg/matrix_io.hh"
#include "linalg/thomas.hh"
#include "params.hh"
#include "util.hh"

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
using dus_type = CyclicList<u_type, which_AB>;
static constexpr real b = dt * c / (2 * dx);

void set_to_initial_conditions(u_type &u) {
    for (int i = 0; i < nx; i++) {
        real x = i * len_x / nx + min_x;
        u(i, 0) = std::sin(2 * M_PI * x);
    }
}

void solve_for_du(const u_type &u, u_type &du) {
    for (int i = 1; i < nx - 1; i++) {
        du(i, 0) = -b * (u(i + 1, 0) - u(i - 1, 0));
    }
    // periodic BCs
    du(0, 0) = -b * (u(1, 0) - u(nx - 1, 0));
    du(nx - 1, 0) = -b * (u(0, 0) - u(nx - 2, 0));
}

void set_initial_dus(dus_type &dus, const u_type &u_initial) {
    for (int i = 0; i < which_AB; i++) {
        dus.set_ptr(i, new u_type());
        solve_for_du(u_initial, dus[i]);
    }
}

void solve_for_next_u(const u_type &u0, const dus_type &dus, u_type &next_u) {
    if constexpr (which_AB == 1) {
        next_u = u0 + dus[0];
    } else if constexpr (which_AB == 2) {
        next_u = u0 + (real(3) * dus[0] - dus[1]) / real(2);
    } else if constexpr (which_AB == 3) {
        next_u = u0 + (real(23) * dus[0] - real(16) * dus[1] + real(5) * dus[2]) / real(12);
    } else if constexpr (which_AB == 4) {
        next_u = u0 + (real(55) * dus[0] - real(59) * dus[1] + real(37) * dus[2] - real(9) * dus[3]) / real(24);
    }
}

void write_params() {
    linalg::FullMatrix params_mat({{c, real(nx), real(nt), min_x, max_x, dx, dt, real(write_every), real(which_AB), cfl}});
    saveMatrix(out_file, params_mat, false);
}

void write_u(const u_type &u) {
    saveMatrix_transpose(out_file, u, true);
}

int main() {
    // Step 0: Initialization
    //  - set u0 to sin(2pi * dx / len_x * j)
    //  - set du0, du1, ... to -b * (u0^ - u0_)

    std::unique_ptr<u_type> this_u(new u_type);
    std::unique_ptr<u_type> next_u(new u_type);

    set_to_initial_conditions(*this_u);

    dus_type dus;
    set_initial_dus(dus, *this_u);

    write_params();

    for (int i = 0; i < nt; i++) {
        if (i % write_every == 0) {
            write_u(*this_u);
        }

        // Step 1: Solve for v
        //  - set v according to which AB method is in use

        solve_for_next_u(*this_u, dus, *next_u);

        // Step 2: Cycle resources
        //  - swap u0 and v
        //  - relabel duk -> du{k+1} cyclicly
        //  - set du0 to -b * (u0^ - u0_)

        this_u.swap(next_u);
        dus.rotate(-1);
        solve_for_du(*this_u, dus[0]);
    }

    write_u(*this_u);

    return 0;
}