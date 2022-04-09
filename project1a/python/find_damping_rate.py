import numpy as np
import sys

from scipy import optimize
from util import reader

########################################################################
# Constants for flags/mods

BARE_OUTPUT_FLAG = "--bare"

BARE_OUTPUT_MODE  = "bare"  # only print relative error
FANCY_OUTPUT_MODE = "fancy" # print more info, with descriptions

mode = FANCY_OUTPUT_MODE

########################################################################
# Check number of arguments

args = sys.argv[1:]

if BARE_OUTPUT_FLAG in args:
    mode = BARE_OUTPUT_MODE
    args.remove(BARE_OUTPUT_FLAG)

if len(args) != 1:
    sys.exit(f"Usage: {sys.argv[0]} path/to/data.csv")

########################################################################
# Load data to plot

path = sys.argv[1]

u, x, t, nu = reader.read_uxtv(path)

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

median_sigma_experimental = np.median(sigma_experimentals) # mean is more affected by outliers
relative_error = abs((sigma_theoretical - median_sigma_experimental) / median_sigma_experimental)

########################################################################
# Print results

if mode == FANCY_OUTPUT_MODE:
    print(f"-- theoretical damping rate = {sigma_theoretical}")
    print(f"-- median exp. damping rate = {median_sigma_experimental}")
    print(f"-- relative error = {relative_error}")
elif mode == BARE_OUTPUT_MODE:
    print(relative_error)