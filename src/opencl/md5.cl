/**
 * MD5 Message-Digest Algorithm, following RFC 1321 available at:
 * https://www.rfc-editor.org/rfc/rfc1321
 */

typedef struct {
    unsigned long size;     // total size of input
    unsigned char input[64]; // current chunk
    unsigned int A;
    unsigned int B;
    unsigned int C;
    unsigned int D;
} md5_context;

unsigned int rotate_left32(const unsigned int x, const size_t n) {
    return (x << n) | (x >> (32 - n));
}

/* Constants used in the MD5 algorithm */
#define MD5_A 0x67452301
#define MD5_B 0xefcdab89
#define MD5_C 0x98badcfe
#define MD5_D 0x10325476

/* Operations used at each 'step' in the algorithm */
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

void md5_init(md5_context *ctx) {
    ctx->size = (unsigned long)0;

    ctx->A = (unsigned int)MD5_A;
    ctx->B = (unsigned int)MD5_B;
    ctx->C = (unsigned int)MD5_C;
    ctx->D = (unsigned int)MD5_D;
}

/**
 * Process a 512 bit chunk of data, passed in as
 * an array of 16 32-bit unsigned integers.
 */
void md5_process(md5_context *ctx, unsigned int *input) {
    unsigned int A = ctx->A;
    unsigned int B = ctx->B;
    unsigned int C = ctx->C;
    unsigned int D = ctx->D;

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

void md5_update(md5_context *ctx, const unsigned char *input, size_t input_len) {
    unsigned int chunk[16]; // 512-bit chunk
    unsigned int offset = ctx->size % 64;
    ctx->size += (unsigned long)input_len;

    for (size_t i = 0; i < input_len; i++) {
        ctx->input[offset++] = input[i];

        // When input becomes full (512 bits), process a chunk
        if (offset % 64 == 0) {
            for (size_t j = 0; j < 16; j++) {
                chunk[j] = (unsigned int)(ctx->input[j * 4]) |
                           (unsigned int)(ctx->input[j * 4 + 1]) << 8 |
                           (unsigned int)(ctx->input[j * 4 + 2]) << 16 |
                           (unsigned int)(ctx->input[j * 4 + 3]) << 24;
            }

            md5_process(ctx, chunk);
            offset = 0;
        }
    }
}

void md5_final(md5_context *ctx, __global unsigned char *output) {
    unsigned int chunk[16];
    unsigned int offset = ctx->size % 64;
    // Append 1 bit (0b1000000)
    ctx->input[offset++] = (unsigned char)0x80;

    // Append 0 bits until the total length in bytes ≡ 56 mod 64
    if (offset > 56) {
        while (offset % 64) {
            ctx->input[offset++] = (unsigned char)0x00;
        }
        for (size_t i = 0; i < 16; i++) {
            chunk[i] = (unsigned int)(ctx->input[i * 4]) |
                       (unsigned int)(ctx->input[i * 4 + 1]) << 8 |
                       (unsigned int)(ctx->input[i * 4 + 2]) << 16 |
                       (unsigned int)(ctx->input[i * 4 + 3]) << 24;
        }
        md5_process(ctx, chunk);
        offset = 0;
    }

    while (offset != 56) {
        ctx->input[offset++] = (unsigned char)0x00;
    }
    for (size_t i = 0; i < 14; i++) {
        chunk[i] = (unsigned int)(ctx->input[i * 4]) |
                   (unsigned int)(ctx->input[i * 4 + 1]) << 8 |
                   (unsigned int)(ctx->input[i * 4 + 2]) << 16 |
                   (unsigned int)(ctx->input[i * 4 + 3]) << 24;
    }
    // Append length of original message to complete last 512-bit chunk
    chunk[14] = (unsigned int)(ctx->size * 8);
    chunk[15] = (unsigned int)((ctx->size * 8) >> 32);

    md5_process(ctx, chunk);

    // Copy to output buffer
    output[0]  = (unsigned char)((ctx->A & 0x000000FF));
    output[1]  = (unsigned char)((ctx->A & 0x0000FF00) >> 8);
    output[2]  = (unsigned char)((ctx->A & 0x00FF0000) >> 16);
    output[3]  = (unsigned char)((ctx->A & 0xFF000000) >> 24);
    output[4]  = (unsigned char)((ctx->B & 0x000000FF));
    output[5]  = (unsigned char)((ctx->B & 0x0000FF00) >> 8);
    output[6]  = (unsigned char)((ctx->B & 0x00FF0000) >> 16);
    output[7]  = (unsigned char)((ctx->B & 0xFF000000) >> 24);
    output[8]  = (unsigned char)((ctx->C & 0x000000FF));
    output[9]  = (unsigned char)((ctx->C & 0x0000FF00) >> 8);
    output[10] = (unsigned char)((ctx->C & 0x00FF0000) >> 16);
    output[11] = (unsigned char)((ctx->C & 0xFF000000) >> 24);
    output[12] = (unsigned char)((ctx->D & 0x000000FF));
    output[13] = (unsigned char)((ctx->D & 0x0000FF00) >> 8);
    output[14] = (unsigned char)((ctx->D & 0x00FF0000) >> 16);
    output[15] = (unsigned char)((ctx->D & 0xFF000000) >> 24);

    return;
}

void compute_md5(const unsigned char *input, const unsigned int input_len, __global unsigned char *output) {
    md5_context ctx;
    md5_init(&ctx);
    md5_update(&ctx, input, input_len);
    md5_final(&ctx, output);
}
