#include "globals.hpp"
#include <cstdio>
#include <cstring>
#include <getopt.h>
#include <memory>
#include <openssl/evp.h>
#include <string>

struct EVP_free {
    void operator()(void *ptr) { EVP_MD_CTX_free((EVP_MD_CTX *)ptr); }
};
template <typename T> using EVP_ptr = std::unique_ptr<T, EVP_free>;

bool compute_hash(const char *input, char hashed[EVP_MAX_MD_SIZE * 2 + 1],
                  const EVP_MD *hash_type) {
    EVP_ptr<EVP_MD_CTX> context(EVP_MD_CTX_new());

    if (context.get() == nullptr) {
        return false;
    }

    if (EVP_DigestInit_ex(context.get(), hash_type, nullptr) == 0) {
        return false;
    }

    if (EVP_DigestUpdate(context.get(), input, strlen(input)) == 0) {
        return false;
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length = 0;

    if (!EVP_DigestFinal_ex(context.get(), hash, &hash_length)) {
        return false;
    }

    for (int i = 0; i < hash_length; i++) {
        sprintf(&hashed[i * 2], "%02x", hash[i]);
    }
    hashed[hash_length * 2] = '\0';

    return true;
}

int quiet_flag = 0;
int num_threads = 1;
int hash_type = -1;
int atk_mode = -1;

char mask_regex[] = "[^\\]?(\?.)";

int main(int argc, char *argv[]) {
    int opt_char;
    while (1) {
        static struct option long_opts[] = {
            {"help", no_argument, 0, 'h'},
            {"quiet", no_argument, 0, 'q'},
            {"hash-type", required_argument, 0, 'm'},
            {"attack-mode", required_argument, 0, 'a'},
            {"threads", required_argument, 0, 't'},
            {"custom-charset1", required_argument, 0, '1'},
            {"custom-charset2", required_argument, 0, '2'},
            {"custom-charset3", required_argument, 0, '3'},
            {"custom-charset4", required_argument, 0, '4'},
            {0, 0, 0, 0}

        };

        int opt_index = 0;
        opt_char =
            getopt_long(argc, argv, "hqm:a:t:1:2:3:4:", long_opts, &opt_index);

        if (opt_char == -1)
            break;

        switch (opt_char) {
        case 0:
            break;
        case 'h':
            fprintf(stdout, "Usage: shikacracker -m <hash_type> -a <attack_mode> "
                            "[options]... hashfile dictionary|mask\n");
            break;
        case 'q':
            quiet_flag = 0;
            break;
        case 'm':
            try {
                hash_type = std::stoi(optarg);
            } catch (std::exception err) {
                fprintf(stderr, "Argument to -m must be an integer\n");
            }
            break;
        case 'a':
            try {
                atk_mode = std::stoi(optarg);
            } catch (std::exception err) {
                fprintf(stderr, "Argument to -a must be an integer\n");
            }
            break;
        case 't':
            try {
                num_threads = std::stoi(optarg);
            } catch (std::exception err) {
                fprintf(stderr, "Argument to -t must be an integer\n");
            }
            break;
        case '?':
            break;
        default:
            abort();
        }
    }
}
