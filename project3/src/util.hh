#pragma once

#include "linalg/fullmatrix.hh"

namespace util {

template <class Matrix>
void apply_nonlinear_op(Matrix &m) {
    for (int i = 0; i < Matrix::nrows; i++) {
        for (int j = 0; j < Matrix::ncols; j++) {
            m(i, j) *= m(i, j).mag2();
        }
    }
}

template <typename T>
T square(const T &s) {
    return s * s;
}

template <class uhat_type>
auto elementwise_mag2(const uhat_type &u) {
    linalg::FullMatrix<uhat_type::nrows, 1, decltype(u(0, 0).real)> u_real;
    for (int i = 0; i < uhat_type::nrows; i++)
        u_real(i, 0) = u(i, 0).mag2();
    return u_real;
}

template <class uhat_type>
auto real_part(const uhat_type &u) {
    linalg::FullMatrix<uhat_type::nrows, 1, decltype(u(0, 0).real)> u_real;
    for (int i = 0; i < uhat_type::nrows; i++)
        u_real(i, 0) = u(i, 0).real;
    return u_real;
}

template <class uhat_type>
auto imag_part(const uhat_type &u) {
    linalg::FullMatrix<uhat_type::nrows, 1, decltype(u(0, 0).real)> u_real;
    for (int i = 0; i < uhat_type::nrows; i++)
        u_real(i, 0) = u(i, 0).imag;
    return u_real;
}

}  // namespace util