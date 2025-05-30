#pragma once
#include "concurrent_set.hpp"
#include "entry_buffer.hpp"
#include <memory>
#include <openssl/evp.h>
#include <string>

struct EVP_free {
    void operator()(void *ptr) { EVP_MD_CTX_free((EVP_MD_CTX *)ptr); }
};

template <typename T> using EVP_ptr = std::unique_ptr<T, EVP_free>;

// Convert hash_mode argument to EVP_MD *
// that can be passed into compute_hash
const EVP_MD *hash_mode_to_EVP_MD();

bool compute_hash(std::string input, std::string &hashed_str,
                  const EVP_MD *hash_type);

bool compute_hash(const char *input, size_t input_len, std::string &hashed_str,
                  const EVP_MD *hash_type);

void hash_thread(entry_buffer<std::string> &buffer,
                 concurrent_set<std::string> &input_hashes,
                 const EVP_MD *hash_type);
