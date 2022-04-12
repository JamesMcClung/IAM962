import sys
import matplotlib.pyplot as plt
import pandas
import seaborn

from util import reader

########################################################################
# Check number of arguments

if len(sys.argv) != 2:
    sys.exit(f"Usage: {sys.argv[0]} path/to/results.csv")

########################################################################
# Load data to plot

path = sys.argv[1]

data = pandas.read_csv(path, sep=" ", header=0)

########################################################################
# Plot the relative error

seaborn.relplot(
    data=data,
    x="cfl",
    y="dr",
    hue="ab",
    style="ab",
    palette=seaborn.color_palette(n_colors=len(set(data.ab))),
)
plt.show()
