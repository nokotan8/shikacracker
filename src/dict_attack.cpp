#include "dict_attack.hpp"
#include "entry_buffer.hpp"
#include "globals.hpp"
#include "hash.hpp"
#include "helpers.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <openssl/evp.h>
#include <optional>
#include <string>
#include <thread>
#include <vector>

void hash_thread(entry_buffer<std::string> &buffer,
                 concurrent_set<std::string> &input_hashes,
                 const EVP_MD *hash_type) {

    std::string hashed_str;
    while (1) {
        std::optional<std::string> input_str = buffer.remove_item();
        if (input_str == std::nullopt)
            break;

        compute_hash(input_str.value(), hashed_str, hash_type);

        if (input_hashes.count(hashed_str)) {
            input_hashes.erase(hashed_str);
            fprintf(stdout, "Reverse of hash %s found: %s\n",
                    hashed_str.c_str(), input_str->c_str());
        }
        if (input_hashes.empty())
            break;
    }

    return;
}

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
