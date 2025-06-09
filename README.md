# Shikacracker

Shikacracker is a password recovery tool inspired by existing software such as
[Hashcat](https://github.com/hashcat/hashcat) and [John the Ripper](https://github.com/openwall/john),
written to teach myself (and others) about password hashing, parallel programming and other relevant concepts.

## Dependencies

- C++17 compiler (or newer)
- OpenCL 2.x or 3.x compatible device
- [OpenCL](https://www.khronos.org/opencl/)

## Build Instructions

This project uses CMake (minimum required version 3.10).

1. **Create a build directory and navigate to it:**

```bash
mkdir build && cd build
```

2. **Configure:**

```bash
cmake ..
```

3. **Build:**

```bash
cmake --build .
```

## Usage

```
Usage: ./shikacracker -m <hash_type> -a <attack_mode> [options]... hashfile dictionary|mask

  -m, --hash-type	Type of hash to use.
  -a, --attack-mode	Attack mode, e.g. dictionary, mask, etc.
  -t, --threads		Number of threads to use for computing hashes (default 2).
  -c, --char-order	File that defines the character order in which password
			candidates for mask attacks should be generated. It should
			contain one character in the first column of each line.
  -[1-4], --custom-charset[1-4]		Define custom charsets to be used in masks.
Help:
  -h, --help		Show this help message and exit.
      --help-hash	Show a list of available hash types and exit.
      --help-mode	Show a list of available attack modes and exit.
      --help-mask	Show information about how to format masks and exit.
```

### Example: Mask attack

```
./shikacracker -m 3 -a 3 hashes.txt ?d?d?d?d?d?d
```

Generate all strings of length 6 where each character is a digit from 0-9, i.e.

```
000000
000001
...
999998
999999
```

### Where is dictionary attack?

Since I haven't implemented GPU acceleration for dictionary attacks, I moved it
to the [cpu-only](https://github.com/nokotan8/shikacracker/tree/cpu-only) branch for now.

## Supported Hashes

- MD5

That's it. Sorry, my bad. I'm working on SHA1 and SHA256 I promise.

## Performance & Benchmarks

See [benckmarks.md](/performance.md). Currently incomplete.

## Legal Disclaimer

This project is provided for educational and ethical research purposes only.
I do not condone or support the use of this software for illegal, malicious,
or unauthorized activities.

Users of this software should:

- Use it only in lawful, ethical, and authorized scenarios (e.g. with explicit
  permission for testing or recovery).

- Use it responsibly and in compliance with all relevant laws and regulations in your jurisdiction.

Shikacracker is licensed under the [MIT License](/LICENSE). Please note that the MIT License
does not provide any warranty or liability protection for misuse of this software.

## Disclaimer 2

While similar in functionality and command-line option syntax, this software does not incorporate or
derive from Hashcat’s source code in any way.
