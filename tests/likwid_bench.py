import argparse
import subprocess


def generate_likwid_bench_command(size_bytes: int, benchmark: str = "store", num_threads: int = 1):
    return f'likwid-bench -t {benchmark} -w S0:{size_bytes}B:{num_threads}'.split(' ')


# this function is independent of the particular benchmark
def parse_bench_output(output: str):
    result = {}

    # we know that the last line is blank
    lines = output.splitlines()[:-1]

    found_first_usable_line = False
    for line in lines:
        if not found_first_usable_line:
            found_first_usable_line = line.startswith('Cycles')

        if found_first_usable_line:
            name, value = line.split(':')
            result[name] = float(value.strip().split(' ')[0])

    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog="likwid_bench",
        description="Collect results from `likwid-bench` across a range of vector sizes"
    )

    parser.add_argument(
        "benchmark",
        type=str,
        help="Which benchmark to collect (see `likwid-bench -a`)"
    )

    parser.add_argument(
        "--num_threads",
        type=int,
        help="Number of threads for the benchmark",
        default=1
    )

    parser.add_argument(
        "--max_size_bytes",
        type=int,
        help="Maximum vector size in bytes",
        default=2**30
    )

    args = parser.parse_args()

    first_experiment = True
    n = 1024

    while n <= args.max_size_bytes:
        cmd = generate_likwid_bench_command(n, args.benchmark, args.num_threads)
        result = subprocess.run(cmd, capture_output=True)

        if result.returncode != 0:
            # this automatically handles the case where likwid is not present on the system
            print(f"Something when wrong when executing command: {cmd}")
            print(result.stderr.decode())

            # I need to signal that there was an error in the benchmark,
            # users of this script should be able to use the exit code for their logic
            exit(1)

        csv = parse_bench_output(result.stdout.decode())

        if first_experiment:
            first_experiment = False
            header = f"# results of benchmarking {args.benchmark} on {args.num_threads} thread(s)"

            print(header)
            print(','.join(csv.keys()))

        print(','.join(map(str, csv.values())))
        n = n * 2
