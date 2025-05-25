#include "dict_attack.hpp"
#include "globals.hpp"
#include "hash.hpp"
#include "helpers.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <openssl/evp.h>
#include <string>

void dict_attack(std::unordered_set<std::string> &hashes) {
    std::ifstream dict_file(dict);

    if (dict_file.is_open()) {
        const EVP_MD *hash_type = hash_mode_to_EVP_MD();
        std::string dict_line;
        std::string hashed_str;
        while (std::getline(dict_file, dict_line)) {
            compute_hash(dict_line, hashed_str, hash_type);
            if (hashes.count(hashed_str)) {
                fprintf(stdout, "Reverse of hash %s found: %s\n",
                        hashed_str.c_str(), dict_line.c_str());
                hashes.erase(hashed_str);
            }
        }

        if (hashes.size() != 0) {
            fprintf(stdout, "Hashes not cracked:\n");
        }
        for (const auto &hash : hashes) {
            fprintf(stdout, "%s\n", hash.c_str());
        }

    } else {
        fprintf(stderr, "Dictionary file %s could not be opened", dict.c_str());
        exit(1);
    }
}
