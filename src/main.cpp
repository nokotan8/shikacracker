#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <memory>
#include <openssl/evp.h>

struct EVP_free {
    void operator()(void *ptr) { EVP_MD_CTX_free((EVP_MD_CTX *)ptr); }
};
template <typename T> using EVP_ptr = std::unique_ptr<T, EVP_free>;

bool compute_hash(const char *input, char hashed[EVP_MAX_MD_SIZE * 2 + 1],
                  const EVP_MD *hash_type) {
    EVP_ptr<EVP_MD_CTX> context(EVP_MD_CTX_new());

    if (context.get() == nullptr) {
        return false;
    }

    if (EVP_DigestInit_ex(context.get(), hash_type, nullptr) == 0) {
        return false;
    }

    if (EVP_DigestUpdate(context.get(), input, strlen(input)) == 0) {
        return false;
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length = 0;

    if (!EVP_DigestFinal_ex(context.get(), hash, &hash_length)) {
        return false;
    }

    for (int i = 0; i < hash_length; i++) {
        sprintf(&hashed[i * 2], "%02x", hash[i]);
    }
    hashed[hash_length * 2] = '\0';

    return true;
}

int main(int argc, char *argv[]) {
    return 0; 
}

