import sys
import numpy as np
import matplotlib.pyplot as plt

from util import reader

########################################################################
# Check number of arguments

if len(sys.argv) != 2:
    sys.exit(f"Usage: {sys.argv[0]} path/to/data.csv")

########################################################################
# Load data to plot

path = sys.argv[1]

u, x, t, c, which_AB, cfl = reader.read_uxtcwC(path)

########################################################################
# Calculate amplitude of wave for each time

amplitudes = (np.max(u, axis=1) - np.min(u, axis=1)) / 2

########################################################################
# Determine when (if at all) the solution explodes

explode_idx = np.argmax(amplitudes > 2 * amplitudes[0])

if explode_idx:
    print(f"Explodes at t={t[explode_idx]}")

########################################################################
# View the time evolution as an animated plot

fig, ax = plt.subplots()
ax.plot(t, amplitudes)

ax.set_title(f"Wave Equation, AB{which_AB}, cfl={cfl:.2}")
ax.set_xlabel("$t$")
ax.set_ylabel("Amplitude")

ax.set_ylim([0, 2])

plt.show()