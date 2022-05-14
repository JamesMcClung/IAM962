import csv
import numpy as np


def _parse_float_or_int(s: str):
    p = float(s)
    if p == int(p):
        p = int(p)
    return p


def _line2nparray(line) -> np.array:
    return np.array(list(map(float, line)))


class Params:
    def __init__(self, param_names: list, num_aux_datas: int):
        self._param_names = param_names
        self.num_aux_datas = num_aux_datas
        self.aux_datas = []

    def parse(self, line_iter):
        for name, val in zip(self._param_names, next(line_iter)):
            self.__dict__[name] = _parse_float_or_int(val)
        self.nt_out = 1 + self.nt // self.write_every

        for _ in range(self.num_aux_datas):
            self.aux_datas.append(_line2nparray(next(line_iter)))


def read_uxtp(path: str, param_names: list, explicit_x: bool = False, num_aux_data: int = 0):
    """Parse u, x, t, and params from the specified output file."""
    with open(path) as file:
        line_iter = csv.reader(file, delimiter=" ")

        # read parameters
        params = Params(param_names, num_aux_data)
        params.parse(line_iter)

        # prep variables
        if explicit_x:
            x = _line2nparray(next(line_iter))
        else:
            x = np.linspace(params.min_x, params.max_x, params.nx, endpoint=False)
        t = np.zeros(params.nt_out)
        u = np.zeros([params.nt_out, params.nx])

        # read data
        i = 0
        for line in line_iter:
            t[i] = i * params.dt * params.write_every
            u[i] = _line2nparray(line)
            i += 1

    return u, x, t, params
