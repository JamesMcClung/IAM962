#pragma once

namespace _params_util {
template <typename T>
constexpr T abs(T t) {
    return t < 0 ? -t : t;
}

template <typename T>
constexpr T min(T t1, T t2) {
    return t1 < t2 ? t1 : t2;
}
}  // namespace _params_util

#ifndef NU
#define NU 1
#endif

#ifndef C
#define C 1
#endif

#ifndef NX
#define NX 128
#endif

#ifndef NT
#define NT 1024
#endif

#ifndef MIN_X
#define MIN_X 0
#endif

#ifndef MAX_X
#define MAX_X 1
#endif

#ifndef CFL
#define CFL 16
#endif

#ifndef DT
#define DT (cfl * _params_util::min(dx / _params_util::abs(c), dx * dx / nu))
#endif

#ifndef WRITE_EVERY
#define WRITE_EVERY 8
#endif

#ifndef OUT_FILE
#define OUT_FILE "out.csv"
#endif