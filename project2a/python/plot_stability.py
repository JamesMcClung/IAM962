import sys

# hacky way to make importing work
project_root = __file__[: __file__.find("project2a") - 1]
sys.path.append(project_root)

from python.util import stability_plotter
from util.param_names import param_names


def title_func(params):
    return f"Burger's Equation, dt={params.dt:.2E}, $\\nu={params.nu}$"


stability_plotter.plot(param_names, title_func)
