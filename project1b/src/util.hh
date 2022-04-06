#pragma once

#include <memory>

namespace detail {
int modPositive(int n, int mod) {
    n = n % mod;
    return n < 0 ? n + mod : n;
}
}  // namespace detail

template <typename T, int size>
class CyclicList {
   private:
    std::unique_ptr<T> vals[size];
    int start_idx = 0;

   public:
    CyclicList() = default;

    T& operator[](int i) const {
        return *vals[detail::modPositive(start_idx + i, size)];
    }

    // adds n to start_idx
    void rotate(int n) {
        start_idx = (start_idx + n) % size;
    }

    void set_ptr(int i, T* val) {
        vals[detail::modPositive(start_idx + i, size)].reset(val);
    }
};