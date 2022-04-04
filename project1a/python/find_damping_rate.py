import numpy as np
import csv
import sys
from scipy import optimize

########################################################################
# Check number of arguments

if len(sys.argv) != 2:
    sys.exit(f"Usage: {sys.argv[0]} path/to/data.csv")

########################################################################
# Load data to plot

file = sys.argv[1]

with open(file) as tsv:
    line_iter = csv.reader(tsv, delimiter = " ")
    
    # read parameters
    params = [float(p) for p in next(line_iter)]
    nu, nx, nt, min_x, max_x, dx, dt, min_x_bc, max_x_bc, write_every = params
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

########################################################################
# Determine the damping rate, sigma

# exact solution to u_t = nu * u_xx
#   with u0 = sin(4πx)
#   is u = sin(4πx)exp(-(4π)^2 * nu * t)
# => theoretical damping rate is sigma = 16π^2 * nu

sigma_theoretical = 16 * np.pi ** 2 * nu

# to find sigma experimentally, fit it to
#   u = u0 * exp(-sigma * t)

sigma_experimentals = []
for j in range(len(x)):
    [_, sigma_experimental], _ = optimize.curve_fit(lambda t, u0, sigma: u0 * np.exp(-sigma * t),  t,  u[:, j], p0=[x[j], sigma_theoretical])
    sigma_experimentals.append(sigma_experimental)

print(sigma_theoretical)
print(np.median(sigma_experimentals))