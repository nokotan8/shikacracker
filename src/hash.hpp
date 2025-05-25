#include <memory>
#include <openssl/evp.h>
#include <string>

struct EVP_free {
    void operator()(void *ptr) { EVP_MD_CTX_free((EVP_MD_CTX *)ptr); }
};

template <typename T> using EVP_ptr = std::unique_ptr<T, EVP_free>;

bool compute_hash(const char *input, int input_len, std::string &hashed_str,
                  const EVP_MD *hash_type);
