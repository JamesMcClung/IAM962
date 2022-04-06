import sys
import matplotlib.pyplot as plt
import matplotlib.animation as animation

from util import reader

########################################################################
# Check number of arguments

if len(sys.argv) != 2:
    sys.exit(f"Usage: {sys.argv[0]} path/to/data.csv")

########################################################################
# Load data to plot

path = sys.argv[1]

u, x, t, c, which_AB = reader.read_uxtcw(path)

########################################################################
# View the time evolution as an animated plot

fig, ax = plt.subplots()
line, = ax.plot(x, u[0])

def animate(i):
    ax.set_title(f"Wave Equation, AB{which_AB}: $c={c}$, $t={t[i]:.2f}$")
    line.set_ydata(u[i])
    return line, ax

ani = animation.FuncAnimation(
    fig, animate, interval=30, frames=len(t), repeat=True, repeat_delay=1000, blit=False)

ax.set_xlabel("$x$")
ax.set_ylabel("$u$")

plt.show()