#!/usr/bin/env python3
import sys
import subprocess


def generate_likwid_bench_command(size_bytes: int):
    return f'likwid-bench -t store -w S0:{size_bytes}B:1'.split(' ')


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
    first_experiment = True
    n = 1024

    while n < 2**30:
        cmd = generate_likwid_bench_command(n)
        result = subprocess.run(cmd, capture_output=True)

        if result.returncode != 0:
            # this automatically handles the case where likwid is not present
            # on the system
            sys.stderr.write(f"Something when wrong when executing command: {cmd}")
            sys.stderr.write(result.stderr.decode())
            exit()

        csv = parse_bench_output(result.stdout.decode())

        if first_experiment:
            first_experiment = False
            print(','.join(csv.keys()))

        # I just dump the contents on stdout, so that further
        # programs can use the contents
        print(','.join(map(str, csv.values())))
        n = n * 2
