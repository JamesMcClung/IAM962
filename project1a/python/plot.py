# from https://matplotlib.org/stable/gallery/animation/simple_anim.html
# and https://stackoverflow.com/questions/11059390/parsing-a-tab-separated-file-in-python
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import csv
import sys

fig, ax = plt.subplots()

# see project1a.hh
lenx = 1
nx = 128
dx = lenx / nx
dt = 0.006103515625
out_every = 8
nt = 1024
nt_out = nt // out_every

x = np.arange(0, lenx, dx)

u = np.zeros([nt_out,nx])
t = np.zeros(nt_out)

# .csv file with data, e.g. "out.csv"
file = sys.argv[1]

with open(f"{file}") as tsv:
    i = 0
    for line in csv.reader(tsv, delimiter = " "):
        t[i] = i * dt * out_every
        u[i] = np.array(line)
        i += 1
        if i >= nt_out:
            break

        
line, = ax.plot(x, u[0])

def animate(i):
    ax.set_title(f"Heat Equation, t={t[i]:.2f}")
    line.set_ydata(u[i])
    return line, ax

ani = animation.FuncAnimation(
    fig, animate, interval=30, frames=nt_out, repeat=True, repeat_delay=1000, blit=False)

ax.set_xlabel("x")
ax.set_ylabel("u")

plt.show()