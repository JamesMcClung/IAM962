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
// Month: May 2022

// This program solves a nonlinear Schrodinger equation,
//     u_t = i*a*u_xx + i*b*|u|^2*u  (a,b real)
//   on the periodic domain [min_x, max_x) using the Fourier-Galerkin
//   method. The linear term is integrated with Adams-Moulton 1-step
//   (AM1), and the nonlinear term with Adams-Bashforth 2-step (AB2).

// Notation:
//   uhat^{n-k}_j     -> uk      (k = 0, 1, ...)
//   uhat^{n-k}_{j-1} -> uk_     (k = 0, 1, ...)
//   uhat^{n-k}_{j+1} -> uk^     (k = 0, 1, ...)
//   uhat^{n+1}_j     -> v
//   uhat = fft(u) as defined by linalg::fft
//   K is the diagonal matrix with imaginary entries
//     i * sf * [0, 1, ..., N/2-1, -N/2, -N/2+1, ..., -2, -1]
//       where sf = "scale factor" = 2*pi/(max_x-min_x)
//     so fft(du/dx) = K * fft(u) = K * uhat
//   ** denotes elementwise multiplication

// AB2 solves u_t = F(u) explicitly as:
//   v - u0 = dt/2 * (3*F(u0) - F(u1))
// For this problem, F(u) = i*b * |u|^2 * u which in Fourier space is
//   F(uhat) = i*b * fft(|ift(uhat)|^2 ** ift(uhat))
//   => v - u0 = 3 * B(u0) - B(u1)
//   where
//     B(uk) = i*b*dt/2 * fft(|ift(uhat)|^2 ** ift(uhat))

// AM1 solves u_t = F(u) implicitly as:
//   v - u0 = dt/2 * (F(v) + F(u0))
// For this problem, F(uhat) = i*a*K^2 * uhat, so
//   (I - i*a*dt/2 * K^2) * v = (I + i*a*dt/2 * K^2) * u0
//   => v - u0 = M*u0 - u0
//   with diagonal matrix
//     M = (I - i*a*dt/2 * K^2)^-1 * (I + i*a*dt/2 * K^2)

// Combining the two solutions by adding them yields
//   v = u0 + (M*u0 - u0) + (3 * B(u0) - B(u1))
//     = M*u0 + 3*B(u0) - B(u1)

////////////////////////////////////////////////////////////////////////
//                              PROGRAM                               //
////////////////////////////////////////////////////////////////////////

static linalg::BandedMatrix<nx, 0, 0, complex> K, M;
using uhat_type = linalg::FullMatrix<nx, 1, complex>;

auto B(const uhat_type &uhat) {
    // B(uk) = i*b*dt/2 * fft(|ift(uhat)|^2 ** ift(uhat))
    auto temp = linalg::ifft(uhat);
    util::apply_nonlinear_op(temp);
    return complex(0, b * dt / 2) * linalg::fft(temp);
}

void initialize_static_matrices() {
    // initialize K
    real sf = 2 * M_PI / len_x;
    for (int k = 0; k < nx / 2; k++)
        K(k, k) = sf * complex(0, k);
    for (int k = nx / 2; k < nx; k++)
        K(k, k) = sf * complex(0, k - nx);

    // initialize M = (I - i*a*dt/2 * K^2)^-1 * (I + i*a*dt/2 * K^2)
    for (int i = 0; i < nx; i++) {
        auto temp = complex(0, a * dt / 2) * util::square(K(i, i));
        M(i, i) = (1 - temp) / (1 + temp);
    }
}

void set_to_initial_conditions(uhat_type &uhat) {
    uhat_type u;
    for (int i = 0; i < nx; i++)
        u(i, 0) = std::sin(min_x + i * dx);
    uhat = linalg::fft(u);
}

void solve_for_next_uhat(const uhat_type &u0, const uhat_type &u1, uhat_type &v) {
    v = M * u0 + 3 * B(u0) - B(u1);
}

void write_params() {
    linalg::FullMatrix params_mat({{a, b, real(nx), real(nt), min_x, max_x, dx, dt, real(write_every)}});
    linalg::saveMatrix(out_file, params_mat, false);
}

void write_u(const uhat_type &u) {
    if constexpr (out_mode.compare("mag2") == 0) {
        linalg::saveMatrix_transpose(out_file, util::elementwise_mag2(u), true);
    } else if constexpr (out_mode.compare("real") == 0) {
        linalg::saveMatrix_transpose(out_file, util::real_part(u), true);
    } else if constexpr (out_mode.compare("imag") == 0) {
        linalg::saveMatrix_transpose(out_file, util::imag_part(u), true);
    }
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

            // cycle resources: last <- this <- next
            last_uhat.swap(this_uhat);
            this_uhat.swap(next_uhat);
        }
        write_u(linalg::ifft(*this_uhat));
    }

    return 0;
}