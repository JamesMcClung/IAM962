#include <cmath>
#include <iostream>
#include <memory>

#include "linalg/fullmatrix.hh"
#include "linalg/lup_decomp.hh"
#include "linalg/matrix_io.hh"
#include "params.hh"
#include "util.hh"

////////////////////////////////////////////////////////////////////////
//                            DESCRIPTION                             //
////////////////////////////////////////////////////////////////////////

// Author: James McClung
// Month: April 2022

// This program solves Burger's equation, u_t + u * u_x = nu * u_xx
//   using collocation on the Chebyshev-Gauss-Lobatto points. The
//   linear term is integrated with Adams-Moulton 2-step (AM2), and the
//   nonlinear term with Adams-Bashforth 2-step (AB2).

// Notation:
//   u^{n-k}_j     -> uk      (k = 0, 1, ...)
//   u^{n-k}_{j-1} -> uk_     (k = 0, 1, ...)
//   u^{n-k}_{j+1} -> uk^     (k = 0, 1, ...)
//   u^{n+1}_j     -> v
//   du = v - u0 for a given term (the linear or nonlinear term)
//   d1 is the matrix used to differentiate a function on the CGL grid
//   ** denotes elementwise multiplication

// AB2 solves u_t = F(u) explicitly as:
//   du = dt/2 * (3*F(u0) - F(u1))
// For this problem, F(u) = -u ** u_x = -u ** (d1 * u), so
//   du = -dt/2 * (3*u0 ** (d1*u0) - u1 ** (d1*u1))
//      = 3*b0 - b1
//   where bk = -dt/2 * uk ** (d1*uk)

// AM1 solves u_t = F(u) implicitly as:
//   du = dt/2 * (F(v) + F(u0))
// For this problem, F(u) = nu * u_xx = nu * d2 * u
//   => du = dt*nu/2 * d2 * (v + u0)
//         = H * v + H * u0
//   where H = dt*nu/2 * d2

// Combining the two solutions by adding them yields
//   v = u0 + (3*b0 - b1) + (H * v + H * u0)
//   => v - H*v = u0 + 3*b0 - b1 + H*u0
//   => v = inv(A) * (u0 + 3*b0 - b1 + H*u0)
//   where A = I - H

// Consequently, the following are stored:
// d1, H
// the LUP decomposition of A
// u0, v
// b0, b1

////////////////////////////////////////////////////////////////////////
//                              PROGRAM                               //
////////////////////////////////////////////////////////////////////////

using u_type = linalg::FullMatrix<nx, 1, real>;

static u_type x, x_actual;
static linalg::FullMatrix<nx, nx, real> d1, H;
static linalg::LUP_Decomp<linalg::FullMatrix<nx, nx, real>> *lupA_ptr;

void calculate_bk(const u_type &uk, u_type &bk) {
    bk = -c * dt / 2 * util::elementwise_product(uk, d1 * uk);
}

real c_bar(int i) {
    if (i == 0 || i == nx - 1)
        return 2;
    return 1;
}

real sq(real a) {
    return a * a;
}

void initialize_static_matrices() {
    // initialize x
    for (int i = 0; i < nx; i++) {
        x(i, 0) = std::cos(i * M_PI / (nx - 1));  // note nx-1 because we need both endpoints (+-1)
        x_actual(i, 0) = (1 + x(i, 0)) / 2;
    }

    // initialize d1 (lots of ifs, but readable and only runs once)
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < nx; j++) {
            if (i != j) {
                real sign = (i + j) % 2 == 0 ? 1 : -1;
                d1(i, j) = sign * c_bar(i) / (c_bar(j) * (x(i, 0) - x(j, 0)));
            } else if (i == 0) {
                d1(i, j) = (1 + 2 * sq(nx - 1)) / 6;
            } else if (i == nx - 1) {
                d1(i, j) = -(1 + 2 * sq(nx - 1)) / 6;
            } else {
                d1(i, j) = -x(i, 0) / (2 - 2 * sq(x(i, 0)));
            }
            d1(i, j) *= 2;  // to account for remapping [0,1] to [-1,1]
        }
    }

    // initialize I (identity matrix)
    linalg::FullMatrix<nx, nx, real> I(0);
    for (int i = 0; i < nx; i++) {
        I(i, i) = 1;
    }

    H = d1 * d1 * dt * nu / 2;
    // handle BCs by zeroing out top and bottom rows
    for (int c = 0; c < nx; c++)
        H(0, c) = H(nx - 1, c) = 0;

    lupA_ptr = new linalg::LUP_Decomp(I - H);
}

void set_to_initial_conditions(u_type &u) {
    for (int i = 0; i < nx; i++) {
        u(i, 0) = std::sin(2 * M_PI * x_actual(i, 0));
    }
    // set them to exactly 0, not just 0 within machine precision
    u(0, 0) = 0;
    u(nx - 1, 0) = 0;
}

void solve_for_next_u(const u_type &u0, const u_type &b0, const u_type &b1, u_type &v) {
    v = lupA_ptr->solve(u0 + 3 * b0 - b1 + H * u0);
}

void write_params() {
    linalg::FullMatrix params_mat({{nu, real(nx), real(nt), dt, real(write_every), cfl}});
    linalg::saveMatrix(out_file, params_mat, false);
}

void write_u(const u_type &u) {
    linalg::saveMatrix_transpose(out_file, u, true);
}

int main() {
    std::unique_ptr<u_type> this_u(new u_type);
    std::unique_ptr<u_type> next_u(new u_type);
    std::unique_ptr<u_type> this_b(new u_type);
    std::unique_ptr<u_type> last_b(new u_type);

    initialize_static_matrices();
    set_to_initial_conditions(*this_u);
    calculate_bk(*this_u, *last_b);

    write_params();
    write_u(x_actual);
    write_u(*this_u);

    for (int w = 0; w < nt / write_every; w++) {
        for (int i = 0; i < write_every; i++) {
            calculate_bk(*this_u, *this_b);
            solve_for_next_u(*this_u, *this_b, *last_b, *next_u);

            // cycle resources
            this_u.swap(next_u);
            last_b.swap(this_b);
        }
        write_u(*this_u);
    }

    return 0;
}