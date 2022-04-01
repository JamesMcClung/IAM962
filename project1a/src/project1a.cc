#include "project1a.hh"

#include <cmath>
#include <iostream>

#include "linalg/fullmatrix.hh"

using u_type = linalg::FullMatrix<nx, 1, real>;

void set_to_initial_conditions(u_type &u) {
    for (int i = 0; i < nx; i++) {
        real x = i * len_x / nx + min_x;
        u(i, 0) = std::sin(4 * M_PI * x);
    }
}

int main() {
    u_type u;
    set_to_initial_conditions(u);

    std::cout << u << "\n";

    return 0;
}