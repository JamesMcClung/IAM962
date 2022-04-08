#pragma once

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
#define CFL 0.1
#endif

#ifndef DT
#define DT (cfl * dx)
#endif

#ifndef WRITE_EVERY
#define WRITE_EVERY 8
#endif

#ifndef OUT_FILE
#define OUT_FILE "out.csv"
#endif

#ifndef WHICH_AB
#define WHICH_AB 4
#endif