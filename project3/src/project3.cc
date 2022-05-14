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
//     u_t = i*a*u_xx + i*b*|u|^2*u - i*V*u  (a,b,V real; V=V(x))
//   on the periodic domain [min_x, max_x) using the Fourier-Galerkin
//   method. The kinetic term is integrated with Adams-Moulton 1-step
//   (AM1), and the nonlinear and potential terms with Adams-Bashforth
//   2-step (AB2).

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
// For this problem, F(u) = i*b * |u|^2 * u - i*V*u which in Fourier
//   space is
//   F(uhat) = i*b * fft(|ift(uhat)|^2 ** ift(uhat)) - i * fft(V ** ift(uhat))
//           = i * fft((b * |ift(uhat)|^2 - V) ** ift(uhat))
//   => v - u0 = 3 * B(u0) - B(u1)
//   where
//     B(uk) = i*dt/2 * fft((b * |ift(uk)|^2 - V) ** ift(uk))

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

using uhat_type = linalg::FullMatrix<nx, 1, complex>;
static linalg::BandedMatrix<nx, 0, 0, complex> K, M;
static uhat_type V(0);

auto B(const uhat_type &uhat) {
    // B(uk) = i*dt/2 * fft((b * |ift(uk)|^2 - V) ** ift(uk))
    auto temp = linalg::ifft(uhat);
    util::apply_nonlinear_op(temp, b, V);
    return complex(0, dt / 2) * linalg::fft(temp);
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

    // initialize V
    if constexpr (potential.compare("none") == 0) {
        // do nothing
    } else if constexpr (potential.compare("wall") == 0) {
        // wall around 3/4 mark
        int wall_start = 3 * nx / 4;
        int wall_end = wall_start + nx / 64;
        real wall_height = 500;
        for (int i = wall_start; i < wall_end; i++)
            V(i, 0) = wall_height;
    } else if constexpr (potential.compare("well") == 0) {
        // well around 3/4 mark
        int well_start = 3 * nx / 4;
        int well_end = well_start + nx / 64;
        real well_height = -500;
        for (int i = well_start; i < well_end; i++)
            V(i, 0) = well_height;
    }
}

void set_to_initial_conditions(uhat_type &uhat) {
    uhat_type u;

    if constexpr (ic.compare("wave") == 0) {
        real k = 4;
        for (int i = 0; i < nx; i++) {
            real x = min_x + i * dx;
            u(i, 0) = complex(std::cos(k * x), std::sin(k * x));
        }
    } else if constexpr (ic.compare("gauss") == 0) {
        real mean = (min_x + max_x) / 2.;
        real std = len_x / 16.;
        for (int i = 0; i < nx; i++) {
            real x = min_x + i * dx;
            u(i, 0) = std::exp(-util::square((x - mean) / std) / 2);
        }
    } else if constexpr (ic.compare("packet") == 0) {
        real mean = (min_x + max_x) / 8.;
        real std = len_x / 32.;
        real k = -16;
        for (int i = 0; i < nx; i++) {
            real x = min_x + i * dx;
            real envelope = std::exp(-util::square((x - mean) / std) / 2);
            u(i, 0) = envelope * complex(std::cos(k * x), std::sin(k * x));
        }
    }

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