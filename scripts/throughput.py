import sys
import pandas as pd
import matplotlib.pyplot as plt


if __name__ == '__main__':
    # I don't care about nice command line argument handling
    # this script is meant to be executed from the CMake
    if len(sys.argv) != 3:
        print("Need exactly 2 input csv files, quitting")
        exit()

    assembly_data = pd.read_csv(sys.argv[1])
    ignore_likwid_data = False

    try:
        likwid_data_filename = sys.argv[2]
        comment_line_count = 0

        with open(likwid_data_filename, 'r') as file:
            for line in file.readlines():
                if line.startswith('#'):
                    comment_line_count += 1
                else:
                    break

        likwid_data = pd.read_csv(sys.argv[2], header=comment_line_count)

    except:
        print('[WARNING]: could not compare it with likwid data')
        ignore_likwid_data = True

    plt.title("Store bandwidth comparison")
    plt.xscale("log", base=2)
    plt.xlabel('bytes')
    plt.ylabel('MByte/s')

    mbytes_per_s = assembly_data['bytes_written'] / assembly_data['fill_time_s'] / (1024.0 * 1024.0)
    plt.scatter(assembly_data['bytes_written'], mbytes_per_s, label='minimal assembly routine', c='blue')

    if not ignore_likwid_data:
        plt.plot(likwid_data['Size (Byte)'], likwid_data['MByte/s'], label='likwid store throughput', c='red')

    plt.legend()
    plt.show()
