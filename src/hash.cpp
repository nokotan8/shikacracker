#include "hash.hpp"
#include "globals.hpp"
#include <cstdio>
#include <openssl/evp.h>
#include <string>

const EVP_MD *hash_mode_to_EVP_MD() {
    switch (hash_mode) {
        case 0:
            return EVP_md5();
        case 1:
            return EVP_sha1();
        case 2:
            return EVP_sha256();
        case 3:
            return EVP_sha512();
        default:
            fprintf(stderr, "Invalid hash type %d", hash_mode);
            exit(1);
    }

    return nullptr;
};

bool compute_hash_openssl(const char *input, size_t input_len,
                          std::string &hashed_str, const EVP_MD *hash_type) {
    EVP_ptr<EVP_MD_CTX> context(EVP_MD_CTX_new());

    if (context.get() == nullptr) {
        return false;
    }

    if (EVP_DigestInit_ex(context.get(), hash_type, nullptr) == 0) {
        return false;
    }

    if (EVP_DigestUpdate(context.get(), input, input_len) == 0) {
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

bool compute_hash(std::string input, std::string &hashed_str,
                  const EVP_MD *hash_type) {
    return compute_hash_openssl(input.c_str(), input.length(), hashed_str,
                                hash_type);
}

bool compute_hash(const char *input, size_t input_len, std::string &hashed_str,
                  const EVP_MD *hash_type) {
    return compute_hash_openssl(input, input_len, hashed_str, hash_type);
}

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
