#include "charsets.hpp"
#include "globals.hpp"
#include "hash_map.hpp"
#include "help.hpp"
#include "mask_attack.hpp"
#include "modes.hpp"
#include "opencl_setup.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <stdexcept>
#include <string>

enum help_types {
    HELP_HASH_TYPE = 1000,
    HELP_ATTACK_MODE,
    HELP_MASK
};

int quiet_flag = 0;
int num_threads = 2;
int hash_mode = -1;
int atk_mode = -1;
std::string dict;

int main(int argc, char *argv[]) {
    int opt_char;
    static struct option long_opts[] = {
        {"help", no_argument, 0, 'h'},
        {"help-hash", no_argument, 0, HELP_HASH_TYPE},
        {"help-mode", no_argument, 0, HELP_ATTACK_MODE},
        {"help-mask", no_argument, 0, HELP_MASK},
        {"quiet", no_argument, 0, 'q'},
        {"hash-type", required_argument, 0, 'm'},
        {"attack-mode", required_argument, 0, 'a'},
        {"threads", required_argument, 0, 't'},
        {"char-order", required_argument, 0, 'c'},
        {"custom-charset1", required_argument, 0, '1'},
        {"custom-charset2", required_argument, 0, '2'},
        {"custom-charset3", required_argument, 0, '3'},
        {"custom-charset4", required_argument, 0, '4'},
        {0, 0, 0, 0}

    };
    while (1) {
        int opt_index = 0;
        opt_char = getopt_long(argc, argv, "hqm:a:t:c:1:2:3:4:", long_opts,
                               &opt_index);

        if (opt_char == -1)
            break;

        switch (opt_char) {
            case 0:
                break;
            case 'h':
                print_help_general(argv[0]);
                return 0;
            case HELP_HASH_TYPE:
                print_help_hash();
                return 0;
            case HELP_ATTACK_MODE:
                print_help_mode();
                return 0;
            case HELP_MASK:
                print_help_mask();
                return 0;
            case 'q':
                quiet_flag = 1;
                break;
            case 'm':
                try {
                    hash_mode = std::stoi(optarg);
                    if (hash_mode < 0) {
                        fprintf(stderr, "Argument to -m must be 0 or more\n");
                        return 1;
                    }
                } catch (std::exception &err) {
                    fprintf(stderr, "Argument to -m must be an integer\n");
                    return 1;
                }
                break;
            case 'a':
                try {
                    atk_mode = std::stoi(optarg);
                    if (atk_mode < 0) {
                        fprintf(stderr, "Argument to -a must be 0 or more\n");
                        return 1;
                    }
                } catch (std::exception &err) {
                    fprintf(stderr, "Argument to -a must be an integer\n");
                    return 1;
                }
                break;
            case 't':
                try {
                    num_threads = std::stoi(optarg);
                    if (num_threads < 1) {
                        fprintf(stderr, "Argument to -t must be 1 or more\n");
                        return 1;
                    }
                } catch (std::exception &err) {
                    fprintf(stderr, "Argument to -t must be an integer\n");
                    return 1;
                }
                break;
            case 'f':
                try {
                    std::filesystem::path freq_file = optarg;
                    order_charsets(freq_file);
                } catch (std::invalid_argument &err) {
                    fprintf(stderr, "Error: %s", err.what());
                }
            case '?':
                break;
            default:
                abort();
        }
    }

    hash_map<bool> input_hashes;
    std::string hash_or_hashfile = argv[argc - 2];
    std::ifstream hash_file(hash_or_hashfile);
    if (hash_file.is_open()) {
        std::string curr_line;
        while (std::getline(hash_file, curr_line)) {
            input_hashes.insert(curr_line.c_str(), true);
        }
    } else {
        input_hashes.insert(hash_or_hashfile.c_str(), true);
    }

    opencl_setup();

    if (atk_mode == 3) { // Mask attack
        const std::string mask = argv[argc - 1];
        std::string kernel_fn_name;
        size_t digest_len;
        const std::string kernel_code =
            get_mask_kernel(hash_mode, kernel_fn_name, &digest_len);
        try {
            mask_attack(mask, input_hashes, digest_len, kernel_fn_name, kernel_code);
        } catch (std::invalid_argument &err) {
            fprintf(stderr, "Error: %s\n", err.what());
        }
    }

    return 0;
}
