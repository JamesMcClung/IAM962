#include "project1a.hh"

#include <iostream>

#include "linalg/fullmatrix.hh"

int main() {
    std::cout << "Hello, Prof. McHugh\n";

    linalg::FullMatrix<3, 4, int> a(3);

    std::cout << a;

    return 0;
}