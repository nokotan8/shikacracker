#include "charsets.hpp"
#include "dict_attack.hpp"
#include "globals.hpp"
#include "helpers.hpp"
#include "mask_attack.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <stdexcept>
#include <string>

enum help_types { HELP_HASH_TYPE = 1000, HELP_ATTACK_MODE };

int quiet_flag = 0;
int num_threads = 2;
int hash_mode = -1;
int atk_mode = -1;
std::string mask;
std::string dict;

int main(int argc, char *argv[]) {
    int opt_char;
    static struct option long_opts[] = {
        {"help", no_argument, 0, 'h'},
        {"help-hash", no_argument, 0, HELP_HASH_TYPE},
        {"help-mode", no_argument, 0, HELP_ATTACK_MODE},
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
                return 1;
            case HELP_HASH_TYPE:
                fprintf(stdout, "hash type help");
                break;
            case HELP_ATTACK_MODE:
                fprintf(stdout, "attack mode help");
                break;
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

    concurrent_set<std::string> input_hashes;
    std::string hash_or_hashfile = argv[argc - 2];
    std::ifstream hash_file(hash_or_hashfile);
    if (hash_file.is_open()) {
        std::string curr_line;
        while (std::getline(hash_file, curr_line)) {
            input_hashes.insert(curr_line);
        }
    } else {
        input_hashes.insert(hash_or_hashfile);
    }

    if (atk_mode == 0) { // Dictionary attack
        dict = argv[argc - 1];
        dict_attack(input_hashes);
    } else if (atk_mode == 3) { // Mask attack
        mask = argv[argc - 1];
        try {
            mask_attack(input_hashes);
        } catch (std::invalid_argument &err) {
            fprintf(stderr, "Error: %s\n", err.what());
        }
    }

    return 0;
}

/* Buffer testing lmao */
// #include <vector>
// #include <iostream>
// #include <optional>
// #include <thread>
// #include <atomic>

// int main() {
//     const int n = 12000;
//     std::atomic_int num = 0;
//     EntryBuffer<int> test(n);
//     std::vector<std::thread> producers(n);
//     std::vector<std::thread> consumers(n * 2);
//     std::vector<int> res(n);
//
//     for (int i = 0; i < n; i++) {
//         producers[i] = std::thread([&test, &num] {
//             for (int j = 0; j < n; j++) {
//                 test.add_item(j);
//                 num++;
//                 if (num == n * n)
//                     test.add_done();
//             }
//         });
//     }
//
//     for (int i = 0; i < n * 2; i++) {
//         consumers[i] = std::thread([&test, &res] {
//             for (int j = 0; j < n * 3 / 2; j++) {
//                 std::optional<int> item = test.remove_item(res);
//                 if (item == std::nullopt) {
//                     break;
//                 }
//             }
//         });
//     }
//
//     for (int i = 0; i < n; i++) {
//         producers[i].join();
//         consumers[i].join();
//     }
//     for (int j = n; j < n * 2; j++) {
//         consumers[j].join();
//     }
//
//     for (int i = 0; i < n; i++) {
//         if (res[i] != n) {
//             std::cout << "you messed up\n";
//             return 1;
//         }
//     }
//     std::cout << "nice\n";
//
//     return 0;
// }
