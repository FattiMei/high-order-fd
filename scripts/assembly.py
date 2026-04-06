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

    for name, local_df in df.groupby('assembly_method'):
        plt.scatter(local_df['nnz'], local_df['assembly_time_s'], label=name)

    plt.legend()
    plt.show()
