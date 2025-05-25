#include "hash.hpp"
#include "globals.hpp"
#include <cstdio>
#include <getopt.h>
#include <string>

bool compute_hash(std::string input, std::string &hashed_str,
                  const EVP_MD *hash_type) {
    EVP_ptr<EVP_MD_CTX> context(EVP_MD_CTX_new());

    if (context.get() == nullptr) {
        return false;
    }

    if (EVP_DigestInit_ex(context.get(), hash_type, nullptr) == 0) {
        return false;
    }

    if (EVP_DigestUpdate(context.get(), input.c_str(), input.size()) == 0) {
        return false;
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length = 0;

    if (!EVP_DigestFinal_ex(context.get(), hash, &hash_length)) {
        return false;
    }

    hashed_str.resize(hash_length * 2 + 1);
    for (unsigned int i = 0; i < hash_length; i++) {
        snprintf(&hashed_str[i * 2], 3, "%02x", hash[i]);
    }
    hashed_str.pop_back();

    return true;
}
