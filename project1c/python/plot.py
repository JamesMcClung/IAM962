import sys
import matplotlib.pyplot as plt
import matplotlib.animation as animation

from util import reader

########################################################################
# Constants for flags/mods

SAVE_PLOT_FLAG = "--save"

SAVE_PLOT_MODE = "save"  # save plot as gif
VIEW_PLOT_MODE = "view"  # view plot, but don't save

mode = VIEW_PLOT_MODE

########################################################################
# Check number of arguments

args = sys.argv[1:]

if SAVE_PLOT_FLAG in args:
    mode = SAVE_PLOT_MODE
    args.remove(SAVE_PLOT_FLAG)

if len(args) != 1:
    sys.exit(f"Usage: {sys.argv[0]} path/to/data.csv")

########################################################################
# Load data to plot

path = args[0]

u, x, t, params = reader.read_uxtp(path)

########################################################################
# View the time evolution as an animated plot

fig, ax = plt.subplots()
(line,) = ax.plot(x, u[0])


def animate(i):
    ax.set_title(f"Burger's Equation, AB{params.which_AB}, cfl={params.cfl}: $t={t[i]:.2f}$")
    line.set_ydata(u[i])
    return line, ax


ani = animation.FuncAnimation(
    fig, animate, interval=30, frames=len(t), repeat=True, repeat_delay=1000, blit=False
)

ax.set_xlabel("$x$")
ax.set_ylabel("$u$")
ax.set_ybound([-1.5, 1.5])

if mode == VIEW_PLOT_MODE:
    plt.show()
else:
    writergif = animation.PillowWriter(fps=30)
    out_name = "out.gif"
    ani.save(out_name, writer=writergif)
    print(f"Saved to {out_name}")
