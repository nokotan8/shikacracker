#include "dict_attack.hpp"
#include "globals.hpp"
#include "helpers.hpp"
#include <cstdio>
#include <fstream>
#include <getopt.h>
#include <string>
#include <unordered_set>

int quiet_flag = 0;
int num_threads = 1;
int hash_type = -1;
int atk_mode = -1;
std::string mask;
std::string dict;

char mask_regex[] = "[^\\]?(\?.)";

int main(int argc, char *argv[]) {
    int opt_char;
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
    while (1) {
        int opt_index = 0;
        opt_char =
            getopt_long(argc, argv, "hqm:a:t:1:2:3:4:", long_opts, &opt_index);

        if (opt_char == -1)
            break;

        switch (opt_char) {
            case 0:
                break;
            case 'h':
                print_help_general();
                return 1;
                break;
            case 'q':
                quiet_flag = 1;
                break;
            case 'm':
                try {
                    hash_type = std::stoi(optarg);
                } catch (std::exception &err) {
                    fprintf(stderr, "Argument to -m must be an integer\n");
                    return 1;
                }
                break;
            case 'a':
                try {
                    atk_mode = std::stoi(optarg);
                } catch (std::exception &err) {
                    fprintf(stderr, "Argument to -a must be an integer\n");
                    return 1;
                }
                break;
            case 't':
                try {
                    num_threads = std::stoi(optarg);
                } catch (std::exception &err) {
                    fprintf(stderr, "Argument to -t must be an integer\n");
                    return 1;
                }
                break;
            case '?':
                break;
            default:
                abort();
        }
    }

    std::unordered_set<std::string> hashes;
    std::string hash_or_hashfile = argv[argc - 2];
    std::ifstream hash_file(hash_or_hashfile);
    if (hash_file.is_open()) {
        std::string curr_line;
        while (std::getline(hash_file, curr_line))
            hashes.insert(trim_str(curr_line));
    } else {
        hashes.insert(hash_or_hashfile);
    }

    if (atk_mode == 0) {
        dict = argv[argc - 1];
        dict_attack(hashes);
    } else if (atk_mode == 3) {
        mask = argv[argc - 1];
    }
}
