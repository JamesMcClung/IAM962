#pragma once

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
#define MAX_X 0
#endif

#ifndef DT
#define USE_DT_OPT
#define DT (dx / c)
#endif

#ifndef WRITE_EVERY
#define WRITE_EVERY 8
#endif

#ifndef OUT_FILE
#define OUT_FILE "out.csv"
#endif