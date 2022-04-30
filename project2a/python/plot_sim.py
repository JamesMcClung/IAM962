from util import sim_plotter

param_names = ["nu", "nx", "nt", "min_x", "max_x", "dx", "dt", "write_every", "cfl"]


def title_func(params, time):
    return f"Burger's Equation, dt={params.dt:.2E}, $\\nu={params.nu}$: $t={time:.2f}$"


sim_plotter.plot(param_names, title_func)
