# Script to calculate the frequencies of characters in
# files(s). Prints characters and their frequencies to
# stdout if they are above a certain frequency, defaulting
# to 1.
#
# Usage: python3 character_frequencies.py <cutoff> files

import sys


def get_char_freqs():
    args = sys.argv
    if args[1] == "-h" or args[1] == "--help":
        print(f"Usage: python3 {args[0]} <cutoff> files")
        exit(0)

    try:
        cutoff = int(args[1])
    except ValueError:
        print("Invalid cutoff provided, defaulting to 1")
        cutoff = 1
    freqs = {}

    for i in range(2, len(args)):
        with open(args[i], "r", errors="ignore") as input_file:
            line = input_file.readline()
            while line:
                for c in line[:-1]:
                    freqs[c] = freqs.get(c, 0) + 1
                line = input_file.readline()

    freqs_list = list(freqs.items())
    freqs_list.sort(key=lambda x: x[1], reverse=True)

    for char, freq in freqs_list:
        if freq < cutoff:
            break
        print(f"{char}\t{freq}")


get_char_freqs()
