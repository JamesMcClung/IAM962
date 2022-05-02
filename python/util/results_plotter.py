import sys
import matplotlib.pyplot as plt
import pandas
import seaborn


def plot(x: str, y: str, classification: str):
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
        x=x,
        y=y,
        hue=classification,
        style=classification,
        palette=seaborn.color_palette(n_colors=len(set(data[classification]))),
    )
    plt.show()
