#include "project1a.hh"

#include <cmath>
#include <iostream>

#include "linalg/bandedmatrix.hh"
#include "linalg/fullmatrix.hh"
#include "linalg/matrix_io.hh"
#include "linalg/thomas.hh"

using u_type = linalg::FullMatrix<nx, 1, real>;
static linalg::BandedMatrix<nx, 1, 1, real> coefs_CN(-1);

void set_to_initial_conditions(u_type &u) {
    for (int i = 0; i < nx; i++) {
        real x = i * len_x / nx + min_x;
        u(i, 0) = std::sin(4 * M_PI * x);
    }
}

void initialize_coefs_CN() {
    // Notation:
    // u^n -> u
    // u^{n+1} -> v
    // u_j -> u
    // u_{j-1} -> u_
    // u_{j+1} -> u^
    // real b = 2 * dx * dx / (dt * nu);

    // Crank-Nicolson scheme can be written as:
    // b(v - u) = v^ - 2v + v_ + u^ - 2u + u_
    // => -v_ + (b+2)v - v^ = u_ + (b-2)u + u^

    // I take dt = dx^2/nu, so b = 2, and C-N becomes:
    // => -v_ + 4v - v^ = u_ + u^
    for (int r = 0; r < nx; r++) {
        // off-diagonals are already -1
        coefs_CN(r, r) = 4;
    }
}

void solve_for_next_u(const u_type &u, u_type &next_u) {
    // see initialize_coefs_CN for derivation of scheme
    for (int r = 1; r < nx - 1; r++) {
        next_u(r, 0) = u(r - 1, 0) + u(r + 1, 0);
    }
    // handle boundaries separately:
    next_u(0, 0) = min_x_bc + u(1, 0);
    next_u(nx - 1, 0) = u(nx - 2, 0) + max_x_bc;

    thomas_in_place(coefs_CN, next_u);
}

void write_u(const u_type &u) {
    static bool first_write = true;

    saveMatrix_transpose(out_file, u, !first_write);
    first_write = false;
}

int main() {
    std::unique_ptr<u_type> this_u(new u_type);
    std::unique_ptr<u_type> next_u(new u_type);

    set_to_initial_conditions(*this_u);
    initialize_coefs_CN();

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