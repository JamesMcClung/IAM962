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
//   d1 is the matrix used to differentiate a function on the CGL grid
//   ** denotes elementwise multiplication

// AB2 solves u_t = F(u) explicitly as:
//   v - u0 = dt/2 * (3*F(u0) - F(u1))
// For this problem, F(u) = -u * u_x, so
//   v - u0 = 3 * B(u0) - B(u1)
//   where
//     B(uk) = -dt/2 * uk ** (d1 * uk)

// AM2 solves u_t = F(u) implicitly as:
//   v - 5*dt/12 * F(v) = u0 + dt/12 * (8*F(u0) - F(u1))
// For this problem, F(u) = nu * u_xx = nu * d1^2 * u, so
//   (I - 5*dt*nu/12 * d1^2) * v = u0 + dt*nu/12 * d1^2 * (8 * u0 - u1)
//   => v - u0 = M0*u0 + M1*u1 - u0
//   with diagonal matrices
//     M0 = (I - 5*dt*nu/12 * d1^2)^-1 * (I + 8*dt*nu/12 * d1^2)
//     M1 = -(I - 5*dt*nu/12 * d1^2)^-1 * dt*nu/12 * d1^2

// Combining the two solutions by adding them yields
//   v = u0 + (M0*u0 + M1*u1 - u0) + (3 * B(u0) - B(u1))
//     = M0*u0 + M1*u1 + 3*B(u0) - B(u1)

////////////////////////////////////////////////////////////////////////
//                              PROGRAM                               //
////////////////////////////////////////////////////////////////////////

using u_type = linalg::FullMatrix<nx, 1, real>;

static u_type x, x_actual;
static linalg::FullMatrix<nx, nx, real> I(0), d1, M0, M1;

auto B(const u_type &uk) {
    return -c * dt / 2 * util::elementwise_product(uk, d1 * uk);
}

real c_bar(int i) {
    if (i == 0 || i == nx - 1)
        return 2;
    return 1;
}

void initialize_static_matrices() {
    // initialize x
    for (int i = 0; i < nx; i++) {
        x(i, 0) = std::cos(i * M_PI / (nx - 1));  // note nx-1 because we need both endpoints (+-1)
        x_actual(i, 0) = (1 + x(i, 0)) / 2;
    }

    // initialize I (identity matrix)
    for (int i = 0; i < nx; i++) {
        I(i, i) = 1;
    }

    // initialize d1 (lots of ifs, but readable and only runs once)
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < nx; j++) {
            if (i != j) {
                d1(i, j) = (i + j % 2 == 0 ? 1 : -1) * c_bar(i) / (c_bar(j) * (x(i, 0) - x(j, 0)));
            } else if (i == 0) {
                d1(i, j) = (1 + 2 * nx * nx) / 6;
            } else if (i == nx - 1) {
                d1(i, j) = -(1 + 2 * nx * nx) / 6;
            } else {
                d1(i, j) = -x(i, 0) / (2 * (1 - x(i, 0) * x(j, 0)));
            }
            d1(i, j) *= 2;  // to account for remapping [0,1] to [-1,1]
        }
    }

    // initialize M0 = (I - 5*dt*nu/12 * d1^2)^-1 * (I + 8*dt*nu/12 * d1^2)
    // and        M1 = -(I - 5*dt*nu/12 * d1^2)^-1 * dt*nu/12 * d1^2
    auto d2 = d1 * d1;
    auto lup = linalg::LUP_Decomp(I - 5 * dt * nu / 12 * d2);
    M0 = lup.solve(I + 8 * dt * nu / 12 * d2);
    M1 = -lup.solve(dt * nu / 12 * d2);
}

void set_to_initial_conditions(u_type &u) {
    for (int i = 0; i < nx; i++) {
        u(i, 0) = std::sin(2 * M_PI * x_actual(i, 0));
    }
}

void solve_for_next_u(const u_type &u0, const u_type &u1, u_type &v) {
    v = M0 * u0 + M1 * u1 + 3 * B(u0) - B(u1);
}

void write_params() {
    linalg::FullMatrix params_mat({{nu, real(nx), real(nt), dt, real(write_every), cfl}});
    linalg::saveMatrix(out_file, params_mat, false);
}

void write_u(const u_type &u) {
    linalg::saveMatrix_transpose(out_file, u, true);
}

int main() {
    std::unique_ptr<u_type> last_u(new u_type);
    std::unique_ptr<u_type> this_u(new u_type);
    std::unique_ptr<u_type> next_u(new u_type);

    initialize_static_matrices();
    set_to_initial_conditions(*last_u);
    set_to_initial_conditions(*this_u);

    write_params();
    write_u(x_actual);
    write_u(*this_u);

    for (int w = 0; w < nt / write_every; w++) {
        for (int i = 0; i < write_every; i++) {
            solve_for_next_u(*this_u, *last_u, *next_u);

            // cycle resources
            // this -> last -> next -> this

            last_u.swap(this_u);
            next_u.swap(this_u);
        }
        write_u(*this_u);
    }

    return 0;
}