/**
 * SHA(Secure Hash Algorithm)256 implementation, following the 
 * Secure Hash Standard available from:
 * https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf
 */

#ifndef INC_SHA256_H
#define INC_SHA256_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t size; // size of the input in bytes
    uint8_t input[64]; // current chunk
    uint32_t state[8]; // hash values
} sha256_context;

/**
 * Initialize SHA256 context. This should be called after
 * instantiating ctx. If ctx was used previously, this
 * this should also be called before reusing ctx for a
 * new hash.
 */
void sha256_init(sha256_context *ctx);

/*
 * Feed input of size input_len into the SHA256 algorithm.
 * This may be called multiple times, i.e. the input
 * string does not have to be passed in all at once.
 */
void sha256_update(sha256_context *ctx, const uint8_t *input, size_t input_len);

/*
 * Finishes running the algorithm and stores the
 * message digest in output. As SHA256 produces an
 * output of 256 bits, output must be able to
 * store at least 32 elements.
 */
void sha256_final(sha256_context *ctx, uint8_t *output);

#ifdef __cplusplus
}
#endif

#endif // INC_SHA256_H
