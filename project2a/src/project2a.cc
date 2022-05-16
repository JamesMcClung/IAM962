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
//   with Adams-Moulton 1-step (AM1), and the nonlinear term with
//   Adams-Bashforth 2-step (AB2).

// Notation:
//   uhat^{n-k}_j     -> uk      (k = 0, 1, ...)
//   uhat^{n-k}_{j-1} -> uk_     (k = 0, 1, ...)
//   uhat^{n-k}_{j+1} -> uk^     (k = 0, 1, ...)
//   uhat^{n+1}_j     -> v
//   uhat = fft(u) as defined by linalg::fft
//   K is the diagonal matrix with imaginary entries
//     2*pi*i * [0, 1, ..., N/2-1, -N/2, -N/2+1, ..., -2, -1]
//     so fft(du/dx) = K * fft(u) = K * uhat (note the factor of 2pi,
//     which comes from the domain being remapped from [0,1] to [0,2pi])
//   ** denotes elementwise multiplication

// AB2 solves u_t = F(u) explicitly as:
//   v - u0 = dt/2 * (3*F(u0) - F(u1))
// For this problem, F(u) = -u * u_x which in Fourier space is
//   F(uhat) = -fft(ift(uhat) ** ift(K * uhat))
//   => v - u0 = 3 * B(u0) - B(u1)
//   where
//     B(uk) = -dt/2 * fft(ift(uk) ** ift(K * uk))

// AM1 solves u_t = F(u) implicitly as:
//   v - u0 = dt/2 * (F(v) + F(u0))
// For this problem, F(uhat) = nu * K^2 * uhat, so
//   v - u0 = nu*dt/2 * K^2 * (v + u0)
//   => v - u0 = M1*v + M1*u0
//   with diagonal matrix
//     M1 = nu*dt/2 * K^2

// Combining the two solutions by adding them yields
//   v = u0 + (M1*v + M1*u0) + (3 * B(u0) - B(u1))
//     => (I-M1)*v = (I+M1)*u0 + 3*B(u0) - B(u1)
//     => v = M2 * (M3*u0 + 3*B(u0) - B(u1))
//   with diagonal matrices
//     M2 = (I-M1)^-1
//     M3 = I+M1

////////////////////////////////////////////////////////////////////////
//                              PROGRAM                               //
////////////////////////////////////////////////////////////////////////

static linalg::BandedMatrix<nx, 0, 0, complex> K, M2, M3;
using uhat_type = linalg::FullMatrix<nx, 1, complex>;

auto B(const uhat_type &uk) {
    // B(uk) = -dt/2 * fft(ift(uk) ** ift(K * uk))
    return -c * dt / 2 * linalg::fft(util::elementwise_product(linalg::ifft(uk), linalg::ifft(K * uk)));
}

void initialize_static_matrices() {
    // initialize K
    for (int k = 0; k < nx / 2; k++)
        K(k, k) = 2 * M_PI * complex(0, k);
    for (int k = nx / 2; k < nx; k++)
        K(k, k) = 2 * M_PI * complex(0, k - nx);

    // initialize M2 = (I-M1)^-1
    // and        M3 = I+M1
    for (int i = 0; i < nx; i++) {
        // had M1 = nu*dt/2 * K^2
        complex m1 = dt * nu * K(i, i) * K(i, i) / 2;
        M2(i, i) = 1 / (1 - m1);
        M3(i, i) = 1 + m1;
    }
}

void set_to_initial_conditions(uhat_type &uhat) {
    // note that sin(x) = 1/2i * exp(ix) - 1/2i * exp(-ix)
    uhat(1, 0) = complex(0, -.5);
    uhat(nx - 1, 0) = complex(0, .5);
}

void solve_for_next_uhat(const uhat_type &u0, const uhat_type &u1, uhat_type &v) {
    v = M2 * (M3 * u0 + 3 * B(u0) - B(u1));
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