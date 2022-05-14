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

#ifndef A
#define A 1
#endif

#ifndef B
#define B 1
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
#define MAX_X (2 * M_PI)
#endif

#ifndef DT
#define DT dx
#endif

#ifndef OUT_MODE
#define OUT_MODE "mag2"
#endif

#ifndef WRITE_EVERY
#define WRITE_EVERY 8
#endif

#ifndef OUT_FILE
#define OUT_FILE "out.csv"
#endif