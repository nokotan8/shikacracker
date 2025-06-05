#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t size; // total size of input
    uint8_t input[64]; // current chunk
    uint32_t A;
    uint32_t B;
    uint32_t C;
    uint32_t D;
} md5_context;

uint32_t rotate_left32(const uint32_t x, const size_t n);

void md5_init(md5_context *ctx);
void md5_update(md5_context *ctx, uint8_t *input, size_t input_len);
void md5_final(md5_context *ctx, uint8_t *output);
