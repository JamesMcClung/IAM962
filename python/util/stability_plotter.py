from operator import methodcaller
import sys
import numpy as np
import matplotlib.pyplot as plt

from scipy import optimize
from . import sim_reader


def plot(param_names: list, title_func):
    ########################################################################
    # Constants for flags/mods

    BARE_OUTPUT_FLAG = "--bare"

    BARE_OUTPUT_MODE = "bare"  # only print damping rate
    FANCY_OUTPUT_MODE = "fancy"  # plot and add descriptions

    mode = FANCY_OUTPUT_MODE

    ########################################################################
    # Check number of arguments

    args = sys.argv[1:]

    if BARE_OUTPUT_FLAG in args:
        mode = BARE_OUTPUT_MODE
        args.remove(BARE_OUTPUT_FLAG)

    if len(args) != 1:
        sys.exit(f"Usage: {sys.argv[0]} path/to/data.csv [--bare]")

    ########################################################################
    # Load data to plot

    path = args[0]

    u, _, t, params = sim_reader.read_uxtp(path, param_names)

    ########################################################################
    # Calculate amplitude of wave for each time

    amplitudes = (np.max(u, axis=1) - np.min(u, axis=1)) / 2

    ########################################################################
    # Determine when (if at all) the solution explodes

    explode_threshold = 10 * amplitudes[0]
    explode_idx = np.argmax(amplitudes > explode_threshold)

    if mode != BARE_OUTPUT_MODE:
        if explode_idx:
            print(f"-- explodes at t = {t[explode_idx]}")
        else:
            print("-- does not explode")

    ########################################################################
    # Determine when (if at all) the solution forms a shock

    shock_threshold = 1.1 * amplitudes[0]
    shock_idx = np.argmax(amplitudes > shock_threshold)

    if mode != BARE_OUTPUT_MODE:
        if shock_idx:
            print(f"-- shock formation at t = {t[shock_idx]}")
        else:
            print("-- does not form shock")

    ########################################################################
    # Determine the damping rate, sigma

    # This is done by fitting amplitudes to A[0] * exp(-sigma * t),
    #   but only considering pre-shock amplitudes

    damp_end_threshold = shock_threshold
    damp_end_idx = np.argmax(amplitudes > damp_end_threshold) or len(t)
    [sigma], _ = optimize.curve_fit(
        lambda t, sigma: amplitudes[0] * np.exp(-sigma * t),
        t[:damp_end_idx],
        amplitudes[:damp_end_idx],
        p0=[0],
    )

    if mode == FANCY_OUTPUT_MODE:
        if sigma > 0:
            print(f"-- damping rate = {sigma:.3}")
        else:
            print(f"-- growth rate ~= {-sigma:.3}")
    elif mode == BARE_OUTPUT_MODE:
        print(sigma)
        sys.exit()

    ########################################################################
    # View the time evolution as an animated plot

    _, ax = plt.subplots()
    end_idx = explode_idx or shock_idx or len(t)
    ax.plot(t[:end_idx], amplitudes[:end_idx], label="Actual")
    ax.plot(
        t[:damp_end_idx],
        amplitudes[0] * np.exp(-sigma * t[:damp_end_idx]),
        label=f"Exponential fit: $A_0 e^{{-\\sigma t}}, \\sigma={sigma:.3f}$",
    )

    ax.legend()
    ax.set_title(title_func(params))
    ax.set_xlabel("$t$")
    ax.set_ylabel("Amplitude")

    plt.show()
