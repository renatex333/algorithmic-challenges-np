from subprocess import run
from time import perf_counter
from matplotlib.pyplot import subplots
from pandas import DataFrame, read_feather
from multiprocessing import cpu_count
import os
import library.graph as graph

def run_executable(executable, arg, mpi = False, src_folder_path = "src"):
    """
    Arguments:
    executable: name of the executable to run
    arg: argument to pass to the executable
    mpi: if True, run the executable with MPI
    src_folder_path: path to the folder where the executable is located relative to the current working directory (starting with "/")
    """

    if not os.path.exists(f"{src_folder_path}/{executable}"):
        raise ValueError(f"Argument 'executable' must be a valid path, not {repr(executable)}")

    start = perf_counter()
    proc = run([f"{src_folder_path}/{executable}", f"{arg}"], capture_output=False).check_returncode() if not mpi else run(["mpirun", "-np", f"{cpu_count()}", f"./src/{executable}", f"{arg}"], capture_output=False).check_returncode()
    end = perf_counter()
    return (end - start)

def run_on_slurm(executable, arg, mpi = False, src_folder_path = "src"):
    """
    Arguments:
    executable: name of the slurm batch configuration file
    arg: argument to pass to the executable
    mpi: NOT USED
    src_folder_path: path to the folder where the slurm batch configuration file is located relative to the current working directory (starting with "/")
    """

    if not os.path.exists(f"{src_folder_path}/{executable}.slurm"):
        raise ValueError(f"Argument 'executable' must be a valid path, not {repr(executable)}")

    start = perf_counter()
    proc = run(["sbatch", "--wait", f"{src_folder_path}/{executable}.slurm", f"{arg}"]).check_returncode()
    end = perf_counter()
    return (end - start)

def save_results(time, num_vertices, algorithm, results_folder_path = "results"):
    """
    Arguments:
    time: time it took to run the executable
    num_vertices: number of vertices in the graph
    algorithm: name of the algorithm used
    results_folder_path: path to the folder where the results will be saved relative to the current working directory (starting with "/")
    """

    if not os.path.exists(results_folder_path):
        raise ValueError(f"Argument 'results_folder_path' must be a valid path, not {repr(results_folder_path)}")

    # Create a pandas dataframe with the results
    df = DataFrame({"time": time, "num_vertices": num_vertices})
    # Save the dataframe to a feather file
    df.to_feather(f"{results_folder_path}/results_{algorithm}.feather")

def run_and_save(algorithms, min_num_vertices, max_num_vertices, step, where_to_run = "slurm", mpi_algorithms = [], results_folder_path = "results", src_folder_path="src"):
    """
    Arguments:
    algorithms: name of the algorithms used
    slurm_files: name of the slurm batch configuration files
    min_num_vertices: minimum number of vertices in the graph
    max_num_vertices: maximum number of vertices in the graph
    step: step between the number of vertices processed
    where_to_run: where to run the executable, either "slurm" or "local"
    mpi_algorithms: list of algorithms that use MPI
    results_folder_path: path to the folder where the results will be saved relative to the current working directory (starting with "/")
    src_folder_path: path to the folder where the executable is located relative to the current working directory (starting with "/")
    """

    if not os.path.exists(results_folder_path):
        raise ValueError(f"Argument 'results_folder_path' must be a valid path, not {repr(results_folder_path)}")
    
    if not os.path.exists(src_folder_path):
        raise ValueError(f"Argument 'src_folder_path' must be a valid path, not {repr(src_folder_path)}")

    if where_to_run not in ["slurm", "local"]:
        raise ValueError(f"Argument 'where_to_run' must be either 'slurm' or 'local', not {repr(where_to_run)}")

    function = run_on_slurm if where_to_run == "slurm" else run_executable
    
    for algorithm in algorithms:
        print("-+-"*30)
        print(f"Algorithm: {algorithm}")
        is_mpi = algorithm in mpi_algorithms
        run_time = []
        run_num_vertices = []
        for graph_num_vertices in range(min_num_vertices, max_num_vertices+step, step):
            max_clique_size = graph.pipeline(num_vertices=graph_num_vertices, conection_prob=0.5)
            t = function(algorithm, max_clique_size, mpi = is_mpi, src_folder_path=src_folder_path)
            run_time.append(t)
            run_num_vertices.append(graph_num_vertices)
        save_results(run_time, run_num_vertices, algorithm, results_folder_path)
    print("-+-"*30)

def plot_results(algorithms, results_folder_path = "results"):
    """
    Arguments:
    algorithms: name of the algorithms used
    results_folder_path: path to the folder where the results are saved relative to the current working directory (starting with "/")
    """

    if not os.path.exists(results_folder_path):
        raise ValueError(f"Argument 'results_folder_path' must be a valid path, not {repr(results_folder_path)}")

    fig, ax = subplots()
    for algorithm in algorithms:
        # Read the results from the corresponding feather file
        df = read_feather(f"{results_folder_path}/results_{algorithm}.feather")
        # Plot the results
        ax.plot(df["num_vertices"], df["time"], label=algorithm)
        ax.set( title="Algorithm Comparison", xlabel="Graph's Number of Vertices", ylabel="Execution time (s)")
        ax.legend(loc="best")
    fig.savefig(f"{results_folder_path}/results.png")