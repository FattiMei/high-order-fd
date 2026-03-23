import sys
import pandas as pd
import matplotlib.pyplot as plt


if __name__ == '__main__':
    # I don't care about nice command line argument handling
    # this script is meant to be executed from the CMake
    if len(sys.argv) != 2:
        print("Need exactly 1 input csv file")
        exit()

    df = pd.read_csv(sys.argv[1])

    plt.title("Sparse matrix assembly")
    plt.xlabel("nnz")
    plt.ylabel("assembly time [s]")
    plt.xscale("log")
    plt.yscale("log")

    plt.scatter(df['nnz'], df['assembly_time_s'], label="SOTA")
    plt.scatter(df['nnz'], df['lower_bound_s'], label="lower bound")
    plt.legend()
    plt.show()
