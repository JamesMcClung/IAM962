import csv
import numpy as np

def _parse_float_or_int(s: str):
    p = float(s)
    if p == int(p):
        p = int(p)
    return p

def read_uxtcwC(path: str):
    '''Parse u, x, t, and nu from the specified output file.'''
    with open(path) as file:
        line_iter = csv.reader(file, delimiter = " ")
        
        # read parameters
        params = [_parse_float_or_int(p) for p in next(line_iter)]
        c, nx, nt, min_x, max_x, dx, dt, write_every, which_AB, cfl = params
        nt_out = 1 + nt // write_every

        # prep variables
        x = np.arange(min_x, max_x, dx)
        t = np.zeros(nt_out)
        u = np.zeros([nt_out, nx])

        # read data
        i = 0
        for line in line_iter:
            t[i] = i * dt * write_every
            u[i] = np.array(line)
            i += 1

    return u, x, t, c, which_AB, cfl