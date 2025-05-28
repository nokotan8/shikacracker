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

void print_help_general() {
    fprintf(stdout, "Usage: ./shikacracker -m <hash_type> -a <attack_mode> "
                    "[options]... hashfile dictionary|mask\n");
}

const EVP_MD *hash_mode_to_EVP_MD() {
    switch (hash_mode) {
        case 0:
            return EVP_md5();
        default:
            fprintf(stderr, "Invalid hash type %d", hash_mode);
            exit(1);
    }
};
