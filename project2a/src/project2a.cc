#include <cmath>
#include <iostream>

#include "linalg/bandedmatrix.hh"
#include "linalg/fft.hh"
#include "linalg/fullmatrix.hh"
#include "linalg/matrix_io.hh"
#include "params.hh"
#include "util.hh"

////////////////////////////////////////////////////////////////////////
//                            DESCRIPTION                             //
////////////////////////////////////////////////////////////////////////

// Author: James McClung
// Month: April 2022

// This program solves Burger's equation, u_t + u * u_x = nu * u_xx
//   using the Fourier-Galerkin method. The linear term is integrated
//   with Adams-Moulton 2-step (AM2), and the nonlinear term with
//   Adams-Bashforth 2-step (AB2).

// Notation:
//   uhat^{n-k}_j     -> uk      (k = 0, 1, ...)
//   uhat^{n-k}_{j-1} -> uk_     (k = 0, 1, ...)
//   uhat^{n-k}_{j+1} -> uk^     (k = 0, 1, ...)
//   uhat^{n+1}_j     -> v
//   uhat = fft(u) as defined by linalg::fft
//   K is the diagonal matrix with imaginary entries
//     i * [0, 1, ..., N/2-1, -N/2, -N/2+1, ..., -2, -1]
//     so fft(du/dx) = K * fft(u) = K * uhat
//   ** denotes elementwise multiplication

// AB2 solves u_t = F(u) explicitly as:
//   v - u0 = dt/2 * (3*F(u0) - F(u1))
// For this problem, F(u) = -u * u_x which in Fourier space is
//   F(uhat) = -fft(ift(uhat) ** ift(K * uhat))
//   => v - u0 = 3 * B(u0) - B(u1)
//   where
//     B(uk) = -dt/2 * fft(ift(uk) ** ift(K * uk))

// AM2 solves u_t = F(u) implicitly as:
//   v - 5*dt/12 * F(v) = u0 + dt/12 * (8*F(u0) - F(u1))
// For this problem, F(uhat) = nu * K^2 * uhat, so
//   (I - 5*dt*nu/12 * K^2) * v = u0 + dt*nu/12 * K^2 * (8 * u0 - u1)
//   => v - u0 = M0*u0 + M1*u1 - u0
//   with diagonal matrices
//     M0 = (I - 5*dt*nu/12 * K^2)^-1 * (I + 8*dt*nu/12 * K^2)
//     M1 = -(I - 5*dt*nu/12 * K^2)^-1 * dt*nu/12 * K^2

// Combining the two solutions by adding them yields
//   v = u0 + (M0*u0 + M1*u1 - u0) + (3 * B(u0) - B(u1))
//     = M0*u0 + M1*u1 + 3*B(u0) - B(u1)

////////////////////////////////////////////////////////////////////////
//                              PROGRAM                               //
////////////////////////////////////////////////////////////////////////

static linalg::BandedMatrix<nx, 0, 0, complex> K, M0, M1;
using uhat_type = linalg::FullMatrix<nx, 1, complex>;

auto B(const uhat_type &uk) {
    // B(uk) = -dt/2 * fft(ift(uk) ** ift(K * uk))
    return -c * dt / 2 * linalg::fft(util::elementwise_product(linalg::ifft(uk), linalg::ifft(K * uk)));
}

void initialize_static_matrices() {
    // initialize K
    for (int k = 0; k < nx / 2; k++)
        K(k, k) = complex(0, k);
    for (int k = nx / 2; k < nx; k++)
        K(k, k) = complex(0, k - nx);

    // initialize M0 = (I - 5*dt*nu/12 * K^2)^-1 * (I + 8*dt*nu/12 * K^2)
    // and        M1 = -(I - 5*dt*nu/12 * K^2)^-1 * dt*nu/12 * K^2
    for (int i = 0; i < nx; i++) {
        auto temp = dt * nu * K(i, i) * K(i, i) / 12;
        M0(i, i) = (1 + 8 * temp) / (1 - 5 * temp);
        M1(i, i) = -temp / (1 - 5 * temp);
    }
}

void set_to_initial_conditions(uhat_type &uhat) {
    // note that sin(x) = 1/2i * exp(ix) - 1/2i * exp(-ix)
    uhat(1, 0) = complex(0, -.5);
    uhat(nx - 1, 0) = complex(0, .5);
}

void solve_for_next_uhat(const uhat_type &u0, const uhat_type &u1, uhat_type &v) {
    v = M0 * u0 + M1 * u1 + 3 * B(u0) - B(u1);
}

void write_params() {
    linalg::FullMatrix params_mat({{nu, real(nx), real(nt), min_x, max_x, dx, dt, real(write_every), cfl}});
    linalg::saveMatrix(out_file, params_mat, false);
}

void write_u(const uhat_type &u) {
    linalg::saveMatrix_transpose(out_file, util::real_part(u), true);
}

int main() {
    std::unique_ptr<uhat_type> last_uhat(new uhat_type);
    std::unique_ptr<uhat_type> this_uhat(new uhat_type);
    std::unique_ptr<uhat_type> next_uhat(new uhat_type);

    set_to_initial_conditions(*last_uhat);
    set_to_initial_conditions(*this_uhat);
    initialize_static_matrices();

    write_params();
    write_u(linalg::ifft(*this_uhat));

    for (int w = 0; w < nt / write_every; w++) {
        for (int i = 0; i < write_every; i++) {
            solve_for_next_uhat(*this_uhat, *last_uhat, *next_uhat);

            // cycle resources
            // this -> last -> next -> this

            last_uhat.swap(this_uhat);
            next_uhat.swap(this_uhat);
        }
        write_u(linalg::ifft(*this_uhat));
    }

    return 0;
}