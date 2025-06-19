/**
 * MD5 Message-Digest Algorithm, following RFC 1321 available at:
 * https://www.rfc-editor.org/rfc/rfc1321
 */

#ifndef INC_MD5_H
#define INC_MD5_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t size;     // total size of input
    uint8_t input[64]; // current chunk
    uint32_t A;
    uint32_t B;
    uint32_t C;
    uint32_t D;
} md5_context;

/**
 * Initialize MD5 context. This should be called after
 * instantiating ctx. If ctx was used previously, this
 * this should also be called before reusing ctx for a
 * new hash.
 */
void md5_init(md5_context *ctx);

/*
 * Feed input of size input_len into the MD5 algorithm.
 * This may be called multiple times, i.e. the input
 * string does not have to be passed in all at once.
 */
void md5_update(md5_context *ctx, uint8_t *input, size_t input_len);

/*
 * Finishes running the algorithm and stores the
 * message digest in output. As MD5 produces an
 * output of 128 bits, output must be able to
 * store at least 16 elements.
 */
void md5_final(md5_context *ctx, uint8_t *output);

#ifdef __cplusplus
}
#endif

#endif // INC_MD5_H
