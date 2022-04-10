import csv
import numpy as np


def read_uxtv(path: str):
    """Parse u, x, t, and nu from the specified output file."""
    with open(path) as file:
        line_iter = csv.reader(file, delimiter=" ")

        # read parameters
        params = [float(p) for p in next(line_iter)]
        nu, nx, nt, min_x, max_x, dx, dt, min_x_bc, max_x_bc, write_every, cfl = params
        nx, nt, write_every = int(nx), int(nt), int(write_every)
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

    return u, x, t, nu
