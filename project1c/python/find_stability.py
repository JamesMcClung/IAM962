import sys
import numpy as np
import matplotlib.pyplot as plt

from scipy import optimize
from util import reader

########################################################################
# Constants for flags/mods

BARE_OUTPUT_FLAG = "--bare"

BARE_OUTPUT_MODE = "bare"  # only print artificial damping rate
FANCY_OUTPUT_MODE = "fancy"  # plot and add descriptions

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

path = args[0]

u, x, t, params = reader.read_uxtp(path)

########################################################################
# Calculate amplitude of wave for each time

amplitudes = (np.max(u, axis=1) - np.min(u, axis=1)) / 2

########################################################################
# Determine when (if at all) the solution explodes

explode_threshold = 10 * amplitudes[0]
explode_idx = np.argmax(amplitudes > explode_threshold)

if explode_idx:
    print(f"-- explodes at t = {t[explode_idx]}")

########################################################################
# Determine the artificial damping rate, sigma

# exact solution to u_t + c * u_x = 0
#   with u0 = A0 * sin(2πx)
#   is u = A0 * sin(2π(x - c*t))
# so amplitude A0 is constant
# => theoretical damping rate is 0

# to find sigma, fit amplitude to
#   A(t) = A0 * exp(-sigma * t)

damp_end_threshold = 4 * explode_threshold
damp_end_idx = np.argmax(amplitudes > damp_end_threshold) or len(t)
[sigma], _ = optimize.curve_fit(
    lambda t, sigma: amplitudes[0] * np.exp(-sigma * t),
    t[:damp_end_idx],
    amplitudes[:damp_end_idx],
    p0=[0],
)

if mode == FANCY_OUTPUT_MODE:
    if sigma > 0:
        print(f"-- artificial damping rate = {sigma:.3}")
    else:
        print(f"-- artificial growth rate >= {-sigma:.3}")
elif mode == BARE_OUTPUT_MODE:
    print(sigma)
    sys.exit()

########################################################################
# View the time evolution as an animated plot

fig, ax = plt.subplots()
ax.plot(t[:damp_end_idx], amplitudes[:damp_end_idx])

ax.set_title(f"Burger's Equation, AB{params.which_AB}, cfl={float(params.cfl):.3}")
ax.set_xlabel("$t$")
ax.set_ylabel("Amplitude")

plt.show()
