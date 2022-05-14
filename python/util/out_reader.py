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
    def __init__(self, param_names: list, aux_data_names: list):
        self._param_names = param_names
        self.aux_data_names = aux_data_names or []
        self.aux_datas = []

    def parse(self, line_iter):
        for name, val in zip(self._param_names, next(line_iter)):
            self.__dict__[name] = _parse_float_or_int(val)
        self.nt_out = 1 + self.nt // self.write_every

        for _ in self.aux_data_names:
            self.aux_datas.append(_line2nparray(next(line_iter)))


def read_uxtp(path: str, param_names: list, explicit_x: bool = False, aux_data_names: list = None):
    """Parse u, x, t, and params from the specified output file."""
    with open(path) as file:
        line_iter = csv.reader(file, delimiter=" ")

        # read parameters
        params = Params(param_names, aux_data_names)
        params.parse(line_iter)

        # prep variables
        if explicit_x:
            x = _line2nparray(next(line_iter))
        else:
            x = np.linspace(params.min_x, params.max_x, params.nx, endpoint=False)
        t = np.zeros(params.nt_out)
        u = np.zeros([params.nt_out, params.nx])

        # read data
        for i, line in enumerate(line_iter):
            t[i] = i * params.dt * params.write_every
            u[i] = _line2nparray(line)

    return u, x, t, params
