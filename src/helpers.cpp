#include "helpers.hpp"
#include "globals.hpp"
#include <cstdio>
#include <cstdlib>
#include <openssl/evp.h>
#include <string>

std::string trim_wspace(const std::string &str, const std::string &whitespace) {
    const auto str_begin = str.find_first_not_of(whitespace);
    if (str_begin == std::string::npos)
        return "";

    const size_t str_end = str.find_last_not_of(whitespace);

    return str.substr(str_begin, str_end - str_begin + 1);
}

void print_help_general(char *arg0) {
    fprintf(
        stdout,
        "Usage: %s -m <hash_type> -a <attack_mode> [options]... hashfile "
        "dictionary|mask\n\n"
        "  -m, --hash-type\tType of hash to use.\n"
        "  -a, --attack-mode\tAttack mode, e.g. dictionary, mask, etc.\n"
        "  -t, --threads\t\tNumber of threads to use for computing hashes "
        "(default 2).\n"
        "\t\t\tFor attack modes involving a mask, this also defines the\n"
        "\t\t\tnumber of threads used for password candidate generation.\n"
        "  -c, --char-order\tFile that defines the character order in "
        "which password\n"
        "\t\t\tcandidates for mask attacks should be generated. It should\n"
        "\t\t\tcontain one character at the start of each line, where the\n"
        "\t\t\tfirst line contains the character that should be considered\n"
        "\t\t\tfirst and so on.\n\n"
        "  -[1-4], --custom-charset[1-4]\t\tDefine custom charsets to be used "
        "in masks.\n"
        "Help:\n"
        "  -h, --help\t\tShow this help message and exit.\n"
        "      --help-hash\tShow a list of available hash types.\n"
        "      --help-mode\tShow a list of available attack modes.\n"
        "      --help-mask\tShow information about how to format masks.\n"

        ,
        arg0);
}

void print_help_hash() {}

void print_help_mode() {}
