import sys


def get_char_freqs():
    args = sys.argv
    freqs = {}

    for i in range(1, len(args)):
        with open(args[i], "r", errors="ignore") as input_file:
            line = input_file.readline()
            while line:
                for c in line[:-1]:
                    freqs[c] = freqs.get(c, 0) + 1
                line = input_file.readline()

    freqs_list = list(freqs.items())
    freqs_list.sort(key=lambda x: x[1], reverse=True)

    for char, freq in freqs_list:
        print(f"{char}\t{freq}")


get_char_freqs()
