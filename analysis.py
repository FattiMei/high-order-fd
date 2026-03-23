import argparse
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def find_monotonic_prefix(seq):
    n = 1

    while n < len(seq):
        if seq[n] > seq[n-1]:
            break
        else:
            n += 1

    return n


def plot_work_error(df):
    plt.title("Performance comparison")
    plt.xlabel("error")
    plt.ylabel("total time [s]")
    for name, local in df.groupby('name'):
        err = local['errnorm'].to_numpy()
        time = local['total_time_s'].to_numpy()
        maxn = find_monotonic_prefix(err)

        plt.loglog(err[:maxn], time[:maxn], label=name, marker='o')

    plt.legend()


def plot_convergence(df):
    plt.title("Convergence study")
    plt.xlabel("dofs")
    plt.ylabel("error")
    for name, local in df.groupby('name'):
        plt.loglog(local['n'], local['errnorm'], label=name)

    plt.legend()



ANALYSIS_MAP = {
    'work-error': plot_work_error,
    'convergence': plot_convergence,
    'assembly': plot_assembly_time,
    'throughput': plot_memory_throughput,
}


if __name__ == '__main__':
    parser = argparse.ArgumentParser('analysis',
                                     'data analysis for solver performance')
    parser.add_argument('csv_file', help='which file to study')
    parser.add_argument('--analysis-type',
                        type=str,
                        help='which analysis to perform on the data',
                        nargs='+',
                        default=[])

    args = parser.parse_args()

    csv_filename = args.csv_file
    df = pd.read_csv(csv_filename)

    needs_plot = False

    for request in args.analysis_type:
        if request in ANALYSIS_MAP:
            needs_plot = True

            plt.figure()
            ANALYSIS_MAP[request](df)

    if needs_plot:
        plt.show()
    else:
        print("No compatible analysis was found")
        print(f"Available are: {ANALYSIS_MAP.keys()}")
