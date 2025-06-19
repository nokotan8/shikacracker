#include "sha256.h"
#include "../bitops.h"

#include <stdio.h>
#include <string.h>

/* SHA256 operations */
#define ch(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define sigma0(x) (rotate_right32(x, 7) ^ rotate_right32(x, 18) ^ ((x) >> 3))
#define sigma1(x) (rotate_right32(x, 17) ^ rotate_right32(x, 19) ^ ((x) >> 10))
#define sum0(x) (rotate_right32(x, 2) ^ rotate_right32(x, 13) ^ rotate_right32(x, 22))
#define sum1(x) (rotate_right32(x, 6) ^ rotate_right32(x, 11) ^ rotate_right32(x, 25))

/* One round in the compression loop */
#define SHA256_ROUND(i, a, b, c, d, e, f, g, h, tmp)                                               \
    {                                                                                              \
        tmp = h + sum1(e) + ch(e, f, g) + k[i] + w[i];                                           \
        h = sum0(a) + maj(a, b, c) + tmp;                                                        \
        d += tmp;                                                                                  \
    }

/* Round constants */
static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

void sha256_process(sha256_context *ctx, uint8_t *input) {
    uint32_t a, b, c, d, e, f, g, h, tmp1, w[64];

    // Copy message into w[0..15]
    for (int i = 0, j = 0; i < 16; i++, j += 4) {
        w[i] = (input[j] << 24) | (input[j + 1] << 16) | (input[j + 2] << 8) | (input[j + 3]);
    }

    for (int i = 16; i < 64; i++) {
        w[i] = sigma1(w[i - 2]) + w[i - 7] + sigma0(w[i - 15]) + w[i - 16];
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    // Compression function loop
    for (int i = 0; i < 64; i += 8) {
        SHA256_ROUND(i + 0, a, b, c, d, e, f, g, h, tmp1);
        SHA256_ROUND(i + 1, h, a, b, c, d, e, f, g, tmp1);
        SHA256_ROUND(i + 2, g, h, a, b, c, d, e, f, tmp1);
        SHA256_ROUND(i + 3, f, g, h, a, b, c, d, e, tmp1);
        SHA256_ROUND(i + 4, e, f, g, h, a, b, c, d, tmp1);
        SHA256_ROUND(i + 5, d, e, f, g, h, a, b, c, tmp1);
        SHA256_ROUND(i + 6, c, d, e, f, g, h, a, b, tmp1);
        SHA256_ROUND(i + 7, b, c, d, e, f, g, h, a, tmp1);
    }

    // Add processed data to the hash value
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

void sha256_init(sha256_context *ctx) {
    ctx->size = (uint64_t)0;

    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

void sha256_update(sha256_context *ctx, const uint8_t *input, size_t input_len) {
    uint64_t offset = ctx->size % 64;
    ctx->size += (uint64_t)input_len;

    for (size_t i = 0; i < input_len; i++) {
        ctx->input[offset++] = input[i];

        if (offset == 64) {
            sha256_process(ctx, ctx->input);
            offset = 0;
        }
    }
}

void sha256_final(sha256_context *ctx, uint8_t *output) {
    uint64_t offset = ctx->size % 64;
    ctx->input[offset++] = (uint8_t)0x80;

    if (offset > 56) {
        while (offset < 64) {
            ctx->input[offset++] = (uint8_t)0x00;
        }

        sha256_process(ctx, ctx->input);
        offset = 0;
    }

    while (offset != 56) {
        ctx->input[offset++] = (uint8_t)0x00;
    }

    uint64_t len_bits = ctx->size * 8;
    // Append length in bits as 64-bit big-endian integer
    ctx->input[56] = (uint8_t)(len_bits >> 56);
    ctx->input[57] = (uint8_t)(len_bits >> 48);
    ctx->input[58] = (uint8_t)(len_bits >> 40);
    ctx->input[59] = (uint8_t)(len_bits >> 32);
    ctx->input[60] = (uint8_t)(len_bits >> 24);
    ctx->input[61] = (uint8_t)(len_bits >> 16);
    ctx->input[62] = (uint8_t)(len_bits >> 8);
    ctx->input[63] = (uint8_t)(len_bits);

    sha256_process(ctx, ctx->input);

    for (int i = 0; i < 4; i++) {
        output[i]      = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
        output[i + 4]  = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
        output[i + 8]  = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
        output[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
        output[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
        output[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
        output[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
        output[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
    }
}
