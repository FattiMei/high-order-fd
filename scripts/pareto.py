import sys
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
    # I don't care about nice command line argument handling
    # this script is meant to be executed from the CMake
    if len(sys.argv) != 2:
        print("Need exactly 1 input csv file")
        exit()

    df = pd.read_csv(sys.argv[1])

    plt.title("Performance comparison")
    plt.xlabel("error")
    plt.ylabel("total time [s]")
    for name, local in df.groupby('name'):
        err = local['errnorm'].to_numpy()
        time = local['total_time_s'].to_numpy()
        maxn = find_monotonic_prefix(err)

        plt.loglog(err[:maxn], time[:maxn], label=name, marker='o')

    plt.legend()
    plt.show()
