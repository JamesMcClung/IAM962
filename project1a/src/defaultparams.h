#pragma once

#ifndef NU
#define NU 1
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
#define CFL 1
#endif

#ifndef DT
#define USE_DT_OPT
#define DT (CFL * dx * dx / nu)
#else
#undef CFL
#define CFL (DT * nu / (dx * dx))
#endif

#ifndef MIN_X_BC
#define MIN_X_BC 0
#endif

#ifndef MAX_X_BC
#define MAX_X_BC 0
#endif

#ifndef WRITE_EVERY
#define WRITE_EVERY 8
#endif

#ifndef OUT_FILE
#define OUT_FILE "out.csv"
#endif