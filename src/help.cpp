#include "help.hpp"
#include "globals.hpp"
#include <cstdio>
#include <openssl/evp.h>

void print_help_general(char *arg0) {
    fprintf(
        stdout,
        "Usage: %s -m <hash_type> -a <attack_mode> [options]... hashfile "
        "dictionary|mask\n\n"
        "  -m, --hash-type\tType of hash to use.\n"
        "  -a, --attack-mode\tAttack mode, e.g. dictionary, mask, etc.\n"
        "  -t, --threads\t\tNumber of threads to use for computing hashes "
        "(default 2).\n"
        "  -c, --char-order\tFile that defines the character order in "
        "which password\n"
        "\t\t\tcandidates for mask attacks should be generated. It should\n"
        "\t\t\tcontain one character in the first column of each line.\n"
        "  -[1-4], --custom-charset[1-4]\t\tDefine custom charsets to be used "
        "in masks.\n"
        "Help:\n"
        "  -h, --help\t\tShow this help message and exit.\n"
        "      --help-hash\tShow a list of available hash types and exit.\n"
        "      --help-mode\tShow a list of available attack modes and exit.\n"
        "      --help-mask\tShow information about how to format masks and "
        "exit.\n",
        arg0);
}

void print_help_hash() {
    fprintf(stdout, "Hash types:\n"
                    "  0\tMD5\n"
                    // "  1\tSHA1\n"
                    // "  2\tSHA256\n"
                    // "  3\tSHA512\n"
            );
}

void print_help_mode() {
    fprintf(stdout,
            "Attack modes:\n"
            // "  0\tDictionary\tHash a list of words specified from a file\n"
            "  3\tMask\t\tGenerate candidates from a format string\n");
}

void print_help_mask() {
    fprintf(
        stdout,
        "Mask format:\n"
        "  A mask attack requires a format string that specifies "
        "the characters\n"
        "  to be used at each position when generating password candidates.\n"
        "  For each position, it is possible to specify a single character,\n"
        "  a built-in charset or a custom charset.\n"
        "  Charsets are specified using '?' followed by a valid charset "
        "specifier.\n"
        "  For example, '?d?d?d?d' will generate all passwords 0000-9999.\n"
        "  For a single character, simply type it into the format string.\n"
        "  To use '?' as the character itself, use '\?\?'.\n\n"

        "Built-in Charsets:\n"
        "  ?l\tabcdefghijklmnopqrstuvwxyz\n"
        "  ?u\tABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
        "  ?d\t0123456789\n"
        "  ?s\t<space>!\"#$%%&'()*+,-./:;<=>?@[\\]^_`{|}~\n"
        "  ?h\t0123456789abcdef\n"
        "  ?H\t0123456789ABCDEF\n"
        "  ?n\t?u?l?d\n"
        "  ?a\t?a?u?l?d\n\n"

        "Custom charsets:\n"
        "  Custom charsets are specified with the "
        "'-[1-4]/--custom-charset[1-4]'\n"
        "  command-line options, and used in the mask string with '?[1-4]'\n\n"

        "For more information about mask attacks, see:\n"
        "https://hashcat.net/wiki/doku.php?id=mask_attack\n\n"
        "Note that while similar, this program does not follow hashcat's\n"
        "syntax exactly, nor does it include all of hashcat's features.\n");
}
