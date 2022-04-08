import csv
import numpy as np

def _parse_float_or_int(s: str):
    p = float(s)
    if p == int(p):
        p = int(p)
    return p

class Params:
    def __init__(self, params_as_strs):
        params = [_parse_float_or_int(p) for p in params_as_strs]
        self.nx, self.nt, self.min_x, self.max_x, self.dx, self.dt, self.write_every, self.which_AB, self.cfl = params
        self.nt_out = 1 + self.nt // self.write_every

def read_uxtwC(path: str):
    '''Parse u, x, t, and nu from the specified output file.'''
    with open(path) as file:
        line_iter = csv.reader(file, delimiter = " ")
        
        # read parameters
        params = Params(next(line_iter))

        # prep variables
        x = np.arange(params.min_x, params.max_x, params.dx)
        t = np.zeros(params.nt_out)
        u = np.zeros([params.nt_out, params.nx])

        # read data
        i = 0
        for line in line_iter:
            t[i] = i * params.dt * params.write_every
            u[i] = np.array(line)
            i += 1

    return u, x, t, params.which_AB, params.cfl