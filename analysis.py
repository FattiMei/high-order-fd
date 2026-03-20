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
    parser.add_argument('--lower-bounds', type=str, help='csv with the results of assemble time')
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

    # third a error-residual plot
    # I want to know how good the residual is at predicting the error
    plt.figure(3)
    plt.title("Is residual a good predictor?")
    plt.xlabel("residual norm")
    plt.ylabel("error norm")
    plt.xscale('log')
    plt.yscale('log')
    for name, local in df.groupby("name"):
        err = local['errnorm'].to_numpy()
        res = local['resnorm'].to_numpy()

        plt.scatter(res, err, label=name)
    plt.legend()

    if args.lower_bounds is not None:
        assemble_time = pd.read_csv(args.lower_bounds)

        plt.figure(4)
        plt.title("Sparse matrix assembly")
        plt.xlabel('nnz')
        plt.ylabel('assemble_time_s')
        plt.xscale('log')
        plt.yscale('log')

        only_stencils = df[df['name'] != 'tridiag']
        plt.scatter(only_stencils['nnz'], only_stencils['assemble_time_s'], label='reference')
        plt.scatter(assemble_time['nnz'], assemble_time['assemble_time_s'], label='lower bound')
        plt.legend()

    plt.show()
