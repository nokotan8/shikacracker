/**
 * MD5 Message-Digest Algorithm, following RFC 1321 available at:
 * https://www.rfc-editor.org/rfc/rfc1321
 */

#include "md5.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void md5_init(md5_context *ctx) {
    if (ctx == NULL) {
        fprintf(stderr, "md5_init: ctx is NULL");
        exit(1);
    }
    memset(ctx, 0, sizeof(md5_context));

    ctx->size = (uint64_t)0;

    ctx->A = (uint32_t)MD5_A;
    ctx->B = (uint32_t)MD5_B;
    ctx->C = (uint32_t)MD5_C;
    ctx->D = (uint32_t)MD5_D;
}

void md5_update(md5_context *ctx, uint8_t *input, size_t input_len) {
    uint32_t process_buf[16];
    uint32_t offset = ctx->size % 64;
    ctx->size += (uint64_t)input_len;

    for (size_t i = 0; i < input_len; i++) {
        ctx->input[offset++] = input[i];

        if (offset % 64 == 0) {
            for (size_t j = 0; j < 16; j++) {
                process_buf[j] = (uint32_t)(ctx->input[j * 4]) |
                                 (uint32_t)(ctx->input[j * 4 + 1]) << 8 |
                                 (uint32_t)(ctx->input[j * 4 + 2]) << 16 |
                                 (uint32_t)(ctx->input[j * 4 + 3]) << 24;
            }
            offset = 0;
        }
    }
}

void md5_final(md5_context *ctx, uint8_t *output) {}

/**
 * Process a 512 bit chunk of data, passed in as
 * an array of 16 32-bit unsigned integers.
 */
void md5_process(md5_context *ctx, uint32_t *input) {
    uint32_t A = ctx->A;
    uint32_t B = ctx->B;
    uint32_t C = ctx->C;
    uint32_t D = ctx->D;

    // 0 ~ 15
    MD5_STEP(MD5_F, A, B, C, D, input[0], 7, 0xd76aa478L);
    MD5_STEP(MD5_F, D, A, B, C, input[1], 12, 0xe8c7b756L);
    MD5_STEP(MD5_F, C, D, A, B, input[2], 17, 0x242070dbL);
    MD5_STEP(MD5_F, B, C, D, A, input[3], 22, 0xc1bdceeeL);
    MD5_STEP(MD5_F, A, B, C, D, input[4], 7, 0xf57c0fafL);
    MD5_STEP(MD5_F, D, A, B, C, input[5], 12, 0x4787c62aL);
    MD5_STEP(MD5_F, C, D, A, B, input[6], 17, 0xa8304613L);
    MD5_STEP(MD5_F, B, C, D, A, input[7], 22, 0xfd469501L);
    MD5_STEP(MD5_F, A, B, C, D, input[8], 7, 0x698098d8L);
    MD5_STEP(MD5_F, D, A, B, C, input[9], 12, 0x8b44f7afL);
    MD5_STEP(MD5_F, C, D, A, B, input[10], 17, 0xffff5bb1L);
    MD5_STEP(MD5_F, B, C, D, A, input[11], 22, 0x895cd7beL);
    MD5_STEP(MD5_F, A, B, C, D, input[12], 7, 0x6b901122L);
    MD5_STEP(MD5_F, D, A, B, C, input[13], 12, 0xfd987193L);
    MD5_STEP(MD5_F, C, D, A, B, input[14], 17, 0xa679438eL);
    MD5_STEP(MD5_F, B, C, D, A, input[15], 22, 0x49b40821L);
    // 16 ~ 31
    MD5_STEP(MD5_G, A, B, C, D, input[1], 5, 0xf61e2562L);
    MD5_STEP(MD5_G, D, A, B, C, input[6], 9, 0xc040b340L);
    MD5_STEP(MD5_G, C, D, A, B, input[11], 14, 0x265e5a51L);
    MD5_STEP(MD5_G, B, C, D, A, input[0], 20, 0xe9b6c7aaL);
    MD5_STEP(MD5_G, A, B, C, D, input[5], 5, 0xd62f105dL);
    MD5_STEP(MD5_G, D, A, B, C, input[10], 9, 0x02441453L);
    MD5_STEP(MD5_G, C, D, A, B, input[15], 14, 0xd8a1e681L);
    MD5_STEP(MD5_G, B, C, D, A, input[4], 20, 0xe7d3fbc8L);
    MD5_STEP(MD5_G, A, B, C, D, input[9], 5, 0x21e1cde6L);
    MD5_STEP(MD5_G, D, A, B, C, input[14], 9, 0xc33707d6L);
    MD5_STEP(MD5_G, C, D, A, B, input[3], 14, 0xf4d50d87L);
    MD5_STEP(MD5_G, B, C, D, A, input[8], 20, 0x455a14edL);
    MD5_STEP(MD5_G, A, B, C, D, input[13], 5, 0xa9e3e905L);
    MD5_STEP(MD5_G, D, A, B, C, input[2], 9, 0xfcefa3f8L);
    MD5_STEP(MD5_G, C, D, A, B, input[7], 14, 0x676f02d9L);
    MD5_STEP(MD5_G, B, C, D, A, input[12], 20, 0x8d2a4c8aL);
    // 32 ~ 47
    MD5_STEP(MD5_H, A, B, C, D, input[5], 4, 0xfffa3942L);
    MD5_STEP(MD5_H, D, A, B, C, input[8], 11, 0x8771f681L);
    MD5_STEP(MD5_H, C, D, A, B, input[11], 16, 0x6d9d6122L);
    MD5_STEP(MD5_H, B, C, D, A, input[14], 23, 0xfde5380cL);
    MD5_STEP(MD5_H, A, B, C, D, input[1], 4, 0xa4beea44L);
    MD5_STEP(MD5_H, D, A, B, C, input[4], 11, 0x4bdecfa9L);
    MD5_STEP(MD5_H, C, D, A, B, input[7], 16, 0xf6bb4b60L);
    MD5_STEP(MD5_H, B, C, D, A, input[10], 23, 0xbebfbc70L);
    MD5_STEP(MD5_H, A, B, C, D, input[13], 4, 0x289b7ec6L);
    MD5_STEP(MD5_H, D, A, B, C, input[0], 11, 0xeaa127faL);
    MD5_STEP(MD5_H, C, D, A, B, input[3], 16, 0xd4ef3085L);
    MD5_STEP(MD5_H, B, C, D, A, input[6], 23, 0x04881d05L);
    MD5_STEP(MD5_H, A, B, C, D, input[9], 4, 0xd9d4d039L);
    MD5_STEP(MD5_H, D, A, B, C, input[12], 11, 0xe6db99e5L);
    MD5_STEP(MD5_H, C, D, A, B, input[15], 16, 0x1fa27cf8L);
    MD5_STEP(MD5_H, B, C, D, A, input[2], 23, 0xc4ac5665L);
    // 48 ~ 63
    MD5_STEP(MD5_I, A, B, C, D, input[0], 6, 0xf4292244L);
    MD5_STEP(MD5_I, D, A, B, C, input[7], 10, 0x432aff97L);
    MD5_STEP(MD5_I, C, D, A, B, input[14], 15, 0xab9423a7L);
    MD5_STEP(MD5_I, B, C, D, A, input[5], 21, 0xfc93a039L);
    MD5_STEP(MD5_I, A, B, C, D, input[12], 6, 0x655b59c3L);
    MD5_STEP(MD5_I, D, A, B, C, input[3], 10, 0x8f0ccc92L);
    MD5_STEP(MD5_I, C, D, A, B, input[10], 15, 0xffeff47dL);
    MD5_STEP(MD5_I, B, C, D, A, input[1], 21, 0x85845dd1L);
    MD5_STEP(MD5_I, A, B, C, D, input[8], 6, 0x6fa87e4fL);
    MD5_STEP(MD5_I, D, A, B, C, input[15], 10, 0xfe2ce6e0L);
    MD5_STEP(MD5_I, C, D, A, B, input[6], 15, 0xa3014314L);
    MD5_STEP(MD5_I, B, C, D, A, input[13], 21, 0x4e0811a1L);
    MD5_STEP(MD5_I, A, B, C, D, input[4], 6, 0xf7537e82L);
    MD5_STEP(MD5_I, D, A, B, C, input[11], 10, 0xbd3af235L);
    MD5_STEP(MD5_I, C, D, A, B, input[2], 15, 0x2ad7d2bbL);
    MD5_STEP(MD5_I, B, C, D, A, input[9], 21, 0xeb86d391L);

    ctx->A += A;
    ctx->B += B;
    ctx->C += C;
    ctx->D += D;
}
