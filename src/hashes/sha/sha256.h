#ifndef INC_SHA256_H
#define INC_SHA256_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t size;
    uint8_t input[64];
    uint32_t state[8];
} sha256_context;

void sha256_init(sha256_context *ctx);

void sha256_update(sha256_context *ctx, const uint8_t *input, size_t input_len);

void sha256_final(sha256_context *ctx, uint8_t *output);

#ifdef __cplusplus
}
#endif

#endif // INC_SHA256_H
