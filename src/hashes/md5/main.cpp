#include "md5.h"
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <openssl/evp.h>
#include <string.h>
#include <string>

int get_rand(int mx) {
    return rand() % mx;
}

/* From src/hash.[h|c]pp */
struct EVP_free {
    void operator()(void *ptr) {
        EVP_MD_CTX_free((EVP_MD_CTX *)ptr);
    }
};

template <typename T> using EVP_ptr = std::unique_ptr<T, EVP_free>;

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

bool compute_hash(const char *input, size_t input_len, std::string &hashed_str,
                  const EVP_MD *hash_type) {
    return compute_hash_openssl(input, input_len, hashed_str, hash_type);
}

void compute_md5(const char *input, const size_t input_len, char *output) {
    md5_context ctx;
    md5_init(&ctx);
    md5_update(&ctx, (uint8_t *)input, input_len);
    uint8_t hashed[16];
    md5_final(&ctx, hashed);

    for (unsigned int i = 0; i < 16; i++) {
        snprintf(&output[i * 2], 3, "%02x", hashed[i]);
    }
    output[32] = '\0';
}

/**
 * Function to test my MD5 implementation.
 * For all input lengths from 1 ~ MAX_LEN,
 * generates EACH_NUM strings where each
 * each character is randomly chosen ASCII
 * from [0, 127). Compares the output of my
 * code with OpenSSL's implementation.
 */
int main(void) {
    size_t MAX_LEN = 2048;
    size_t EACH_NUM = 200;
    char input[MAX_LEN + 1];

    char my_output[32];
    std::string openssl_output;
    for (size_t i = 1; i < MAX_LEN; i++) {
        for (size_t j = 0; j < EACH_NUM; j++) {
            for (size_t k = 0; k <= i; k++) {
                input[k] = get_rand(127);
            }

            compute_hash(input, i, openssl_output, EVP_md5());
            compute_md5(input, i, my_output);
            if (strcmp(my_output, openssl_output.c_str())) {
                fprintf(stdout, "OpenSSL: %s, Mine: %s\n",
                        openssl_output.c_str(), my_output);
            }
        }
    }

    return 0;
}
