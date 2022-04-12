# Project 1b: Wave Equation via Adams-Bashforth

## Overview

The main function is in `src/project1b.cc`.

### Bash Scripts

- `scripts/build.sh` builds the source code. It takes definable parameters as arguments.
- `scripts/run.sh` runs the simulation, once it is built. Afterwards, a csv file containing the run data should appear.
- `scripts/build_and_run.sh` combines the two previous scripts.
- `scripts/analyze.sh` builds, runs, and finds the stability of _many_ different parameter combinations. Afterwards, a csv file containing the results should appear.

### Python Scripts

- `python/find_stability.py` calculates the artificial damping rate for a given output.
- `python/plot.py` produces an animated plot of a given output. One such result is *wave.gif*.
- `python/plot_analysis.py` displays a scatterplot of the results of `scripts/analyze.sh`. One such result is *dr_vs_cfl.png*.

## Stability

To measure stability, I fit wave amplitude `A(t) = 1/2 * [max(u(x, t)) - min(u(x, t))]` to `A(t) = A(0) * exp(-sigma*t)` to find the artificial damping rate, `sigma`. If a run is unstable, it will have a negative damping rate and eventually explode; in such cases, I only fit A(t) on a region where the amplitude is still reasonably finite.

For some reason, Adams-Bashforth 2 is more stable than Adams-Bashforth 4 (see the scatterplots below). If I made a mistake---and I assume did---I cannot find it.

## wave.gif

This is a sample gif of the time evolution of the wave equation. This run used all default parameters; in particular, `which_AB=4`, `nx=128`, and `dt=0.1*dx/c`. Runs generally looked like this, just sped up or slowed down and with more or less artificial damping. Some runs exploded, of course.

![](wave.gif)

## dr_vs_cfl.png

This is a scatterplot of damping rate versus Courant number: `cfl = dt * c / dx`, and it can be used to calculate `dt` instead of setting `dt` directly. In all these runs, `dx=1/128`, although the grid spacing didn't seem to matter (compare to *dr_vs_cfl_256.png*, which had `dx=256`).

Apparently, `cfl` greater than about 1/2 is unstable, with AB4 exploding faster than AB2. I was worried that I implemented AB4 wrong, but AB5 proved to be even more unstable.

![](dr_vs_cfl.png)

## dr_vs_cfl_zoom.png

This is another scatterplot of damping rate versus Courant number, but for small values of the latter. It shows that AB3, AB4, and AB5 all have essentially the same artificial damping, but AB2 has less. (The damping rate of AB1 is generally negative, i.e. unstable.)

![](dr_vs_cfl_zoom.png)