import sys

# hacky way to make importing work
project_root = __file__[: __file__.find("project2b") - 1]
sys.path.append(project_root)

from python.util import out_plotter
from util.param_names import param_names


def title_func(params, time):
    return f"Burger's Equation, dt={params.dt:.2E}, $\\nu={params.nu}$: $t={time:.2f}$"


out_plotter.plot(param_names, title_func, explicit_x=True)
