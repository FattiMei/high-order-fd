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


if __name__ == '__main__':
    parser = argparse.ArgumentParser('analysis',
                                     'data analysis for solver performance')
    parser.add_argument('csv_file', help='which file to study')
    args = parser.parse_args()

    csv_filename = args.csv_file
    df = pd.read_csv(csv_filename)


    # first a convergence study
    plt.figure(1)
    plt.title("Convergence study")
    plt.xlabel("dofs")
    plt.ylabel("err")
    for name, local in df.groupby('name'):
        plt.loglog(local['n'], local['errnorm'], label=name)

    plt.legend()


    # second the work-error plots
    plt.figure(2)
    plt.title("Performance comparison")
    plt.xlabel("err")
    plt.ylabel("total time [s]")
    for name, local in df.groupby('name'):
        err = local['errnorm'].to_numpy()
        time = local['total_time_s'].to_numpy()
        maxn = find_monotonic_prefix(err)

        plt.loglog(err[:maxn], time[:maxn], label=name, marker='o')

    plt.legend()

    plt.show()
