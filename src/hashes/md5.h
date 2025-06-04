#include <stddef.h>
#include <stdint.h>

#define MD5_A 0x67425301
#define MD5_B 0xedfcba45
#define MD5_C 0x98cbadfe
#define MD5_D 0x13dce476

#define MD5_F(b, c, d) (((b) & (c)) | (~(b) & (d)))
#define MD5_G(b, c, d) (((b) & (d)) | ((c) & ~(d)))
#define MD5_H(b, c, d) ((b) ^ (c) ^ (d))
#define MD5_I(b, c, d) ((c) ^ ((b) | ~(d)))

/**
 * op: one of MD5_[F|G|H|I]
 * w: 32-bit word of 512-bit chunk
 * s: number of times to left rotate
 * t: constant used in the MD5 algorithm
 */
#define MD5_STEP(op, a, b, c, d, w, s, t)                                      \
    {                                                                          \
        a += ((w) + (t) + op((b), (c), (d)));                                  \
        a = rotate_left32(a, s);                                               \
        a += b;                                                                \
    }

typedef struct {
    uint64_t size; // total size of input
    uint8_t input[64];
    uint32_t A;
    uint32_t B;
    uint32_t C;
    uint32_t D;
} md5_context;

unsigned int rotate_left32(const uint32_t x, const int n);

void md5_init(md5_context *ctx);
void md5_update(md5_context *ctx, uint8_t *input, size_t input_len);
void md5_final(md5_context *ctx, uint8_t *output);
// void md5_process(md5_context *ctx, uint8_t *input);
