# from https://matplotlib.org/stable/gallery/animation/simple_anim.html
# and https://stackoverflow.com/questions/11059390/parsing-a-tab-separated-file-in-python
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import csv
import sys

if len(sys.argv) != 2:
    sys.exit(f"Usage: {sys.argv[0]} path/to/data.csv")

file = sys.argv[1]

with open(file) as tsv:
    i = 0
    line_iter = csv.reader(tsv, delimiter = " ")
    
    # read parameters
    params = [float(p) for p in next(line_iter)]
    nu, nx, nt, min_x, max_x, dx, dt, min_x_bc, max_x_bc, write_every = params
    nx, nt, write_every = int(nx), int(nt), int(write_every)
    nt_out = nt // write_every

    x = np.arange(min_x, max_x, dx)

    u = np.zeros([nt_out, nx])
    t = np.zeros(nt_out)

    # read data
    for line in line_iter:
        t[i] = i * dt * write_every
        u[i] = np.array(line)
        i += 1
        if i >= nt_out:
            break

fig, ax = plt.subplots()
line, = ax.plot(x, u[0])

def animate(i):
    ax.set_title(f"Heat Equation, $\\nu$={nu}, t={t[i]:.2f}")
    line.set_ydata(u[i])
    return line, ax

ani = animation.FuncAnimation(
    fig, animate, interval=30, frames=nt_out, repeat=True, repeat_delay=1000, blit=False)

ax.set_xlabel("x")
ax.set_ylabel("u")

plt.show()