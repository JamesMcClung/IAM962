import csv
import numpy as np


def _parse_float_or_int(s: str):
    p = float(s)
    if p == int(p):
        p = int(p)
    return p


class Params:
    def __init__(self, *param_names):
        self._param_names = param_names

    def parse(self, params_as_strs):
        for name, val in zip(self._param_names, params_as_strs):
            self.__dict__[name] = _parse_float_or_int(val)
        self.nt_out = 1 + self.nt // self.write_every


def read_uxtp(path: str, param_names: list):
    """Parse u, x, t, and params from the specified output file."""
    with open(path) as file:
        line_iter = csv.reader(file, delimiter=" ")

        # read parameters
        params = Params(*param_names)
        params.parse(next(line_iter))

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

    return u, x, t, params
