#include "dict_attack.hpp"
#include "globals.hpp"
#include "hash.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <openssl/evp.h>
#include <string>
#include <thread>
#include <vector>

void dict_attack(concurrent_set<std::string> &input_hashes) {
    std::ifstream dict_file(dict);
    if (dict_file.is_open()) {
        entry_buffer<std::string> buffer(num_threads * 2);
        std::vector<std::thread> cracker_threads(num_threads);
        const EVP_MD *hash_mode = hash_mode_to_EVP_MD();
        std::string dict_line;

        for (int i = 0; i < num_threads; i++) {
            cracker_threads[i] = std::thread(hash_thread, std::ref(buffer),
                                             std::ref(input_hashes), hash_mode);
        }

        while (std::getline(dict_file, dict_line)) {
            if (input_hashes.empty()) {
                buffer.finished_add();
                break;
            }
            buffer.add_item(dict_line);
        }
        buffer.finished_add();

        for (int i = 0; i < num_threads; i++) {
            cracker_threads[i].join();
        }

        if (input_hashes.size() != 0) {
            fprintf(stdout, "Hashes not cracked:\n");
            for (const auto &hash : input_hashes.set) {
                fprintf(stdout, "%s\n", hash.c_str());
            }
        }
    } else {
        fprintf(stderr, "Dictionary file %s could not be opened", dict.c_str());
        exit(1);
    }

    return;
}
