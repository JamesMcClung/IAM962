#pragma once

#include "linalg/fullmatrix.hh"
#include "linalg/recursivematrix.hh"

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

}  // namespace util

namespace linalg {
template <int nx, int nblocks, typename scalar_t>
FullMatrix<nx, 1, scalar_t> operator*(const RecursiveMatrix<nx, nx, nblocks, nblocks, scalar_t> &rmat, const FullMatrix<nx, 1, scalar_t> &fvec) {
    FullMatrix<nx, 1, scalar_t> ret;
    for (int r = 0; r < nx; r++) {
        ret(r, 0) = 0;
        for (int i = 0; i < nx; i++) {
            ret(r, 0) += rmat(r, i) * fvec(i, 0);
        }
    }
    return ret;
}
}