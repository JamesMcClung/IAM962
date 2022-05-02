import sys

# hacky way to make importing work
project_root = __file__[: __file__.find("project2a") - 1]
sys.path.append(project_root)

from python.util import results_plotter


results_plotter.plot("nx", "meantime", "nruns")
