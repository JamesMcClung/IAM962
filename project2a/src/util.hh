#pragma once

#include "linalg/fullmatrix.hh"

namespace util {

template <class M1, class M2>
auto elementwise_product(const M1 &m1, const M2 &m2) {
    static_assert(M1::nrows == M2::nrows, "Cannot take elementwise product if dimensions don't match");
    static_assert(M1::ncols == M2::ncols, "Cannot take elementwise product if dimensions don't match");

    linalg::FullMatrix<M1::nrows, M1::ncols, decltype(m1(0, 0) * m2(0, 0))> res;

    for (int i = 0; i < M1::nrows; i++) {
        for (int j = 0; j < M1::ncols; j++) {
            res(i, j) = m1(i, j) * m2(i, j);
        }
    }

    return res;
}

template <class uhat_type>
auto real_part(const uhat_type &u) {
    linalg::FullMatrix<uhat_type::nrows, 1, decltype(u(0, 0).real)> u_real;
    for (int i = 0; i < uhat_type::nrows; i++)
        u_real(i, 0) = u(i, 0).real;
    return u_real;
}

}  // namespace util