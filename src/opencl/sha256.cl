/**
 * SHA(Secure Hash Algorithm)256 implementation, following the 
 * Secure Hash Standard available from:
 * https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf
 * For commented code without unrolled loops, see src/hashes/sha/sha256.*
 *
 * Dependencies:
 *  bitops.cl
 */

typedef struct {
    unsigned long size;
    unsigned char input[64];
    unsigned int state[8];
} sha256_context;

#define ch(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define sigma0(x) (rotate_right32(x, 7) ^ rotate_right32(x, 18) ^ ((x) >> 3))
#define sigma1(x) (rotate_right32(x, 17) ^ rotate_right32(x, 19) ^ ((x) >> 10))
#define sum0(x) (rotate_right32(x, 2) ^ rotate_right32(x, 13) ^ rotate_right32(x, 22))
#define sum1(x) (rotate_right32(x, 6) ^ rotate_right32(x, 11) ^ rotate_right32(x, 25))

#define SHA256_ROUND(i, a, b, c, d, e, f, g, h, tmp)                                               \
    {                                                                                              \
        tmp = h + sum1(e) + ch(e, f, g) + k[i] + w[i];                                           \
        h = sum0(a) + maj(a, b, c) + tmp;                                                        \
        d += tmp;                                                                                  \
    }

__constant static const unsigned int k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

void sha256_process(sha256_context *ctx, unsigned char *input) {
    unsigned int a, b, c, d, e, f, g, h, tmp1, w[64];

    w[0]  = (input[0] << 24)  | (input[1] << 16)  | (input[2] << 8)  | (input[3]);
    w[1]  = (input[4] << 24)  | (input[5] << 16)  | (input[6] << 8)  | (input[7]);
    w[2]  = (input[8] << 24)  | (input[9] << 16)  | (input[10] << 8) | (input[11]);
    w[3]  = (input[12] << 24) | (input[13] << 16) | (input[14] << 8) | (input[15]);
    w[4]  = (input[16] << 24) | (input[17] << 16) | (input[18] << 8) | (input[19]);
    w[5]  = (input[20] << 24) | (input[21] << 16) | (input[22] << 8) | (input[23]);
    w[6]  = (input[24] << 24) | (input[25] << 16) | (input[26] << 8) | (input[27]);
    w[7]  = (input[28] << 24) | (input[29] << 16) | (input[30] << 8) | (input[31]);
    w[8]  = (input[32] << 24) | (input[33] << 16) | (input[34] << 8) | (input[35]);
    w[9]  = (input[36] << 24) | (input[37] << 16) | (input[38] << 8) | (input[39]);
    w[10] = (input[40] << 24) | (input[41] << 16) | (input[42] << 8) | (input[43]);
    w[11] = (input[44] << 24) | (input[45] << 16) | (input[46] << 8) | (input[47]);
    w[12] = (input[48] << 24) | (input[49] << 16) | (input[50] << 8) | (input[51]);
    w[13] = (input[52] << 24) | (input[53] << 16) | (input[54] << 8) | (input[55]);
    w[14] = (input[56] << 24) | (input[57] << 16) | (input[58] << 8) | (input[59]);
    w[15] = (input[60] << 24) | (input[61] << 16) | (input[62] << 8) | (input[63]);

    w[16] = sigma1(w[14]) + w[9]  + sigma0(w[1])  + w[0];
    w[17] = sigma1(w[15]) + w[10] + sigma0(w[2])  + w[1];
    w[18] = sigma1(w[16]) + w[11] + sigma0(w[3])  + w[2];
    w[19] = sigma1(w[17]) + w[12] + sigma0(w[4])  + w[3];
    w[20] = sigma1(w[18]) + w[13] + sigma0(w[5])  + w[4];
    w[21] = sigma1(w[19]) + w[14] + sigma0(w[6])  + w[5];
    w[22] = sigma1(w[20]) + w[15] + sigma0(w[7])  + w[6];
    w[23] = sigma1(w[21]) + w[16] + sigma0(w[8])  + w[7];
    w[24] = sigma1(w[22]) + w[17] + sigma0(w[9])  + w[8];
    w[25] = sigma1(w[23]) + w[18] + sigma0(w[10]) + w[9];
    w[26] = sigma1(w[24]) + w[19] + sigma0(w[11]) + w[10];
    w[27] = sigma1(w[25]) + w[20] + sigma0(w[12]) + w[11];
    w[28] = sigma1(w[26]) + w[21] + sigma0(w[13]) + w[12];
    w[29] = sigma1(w[27]) + w[22] + sigma0(w[14]) + w[13];
    w[30] = sigma1(w[28]) + w[23] + sigma0(w[15]) + w[14];
    w[31] = sigma1(w[29]) + w[24] + sigma0(w[16]) + w[15];
    w[32] = sigma1(w[30]) + w[25] + sigma0(w[17]) + w[16];
    w[33] = sigma1(w[31]) + w[26] + sigma0(w[18]) + w[17];
    w[34] = sigma1(w[32]) + w[27] + sigma0(w[19]) + w[18];
    w[35] = sigma1(w[33]) + w[28] + sigma0(w[20]) + w[19];
    w[36] = sigma1(w[34]) + w[29] + sigma0(w[21]) + w[20];
    w[37] = sigma1(w[35]) + w[30] + sigma0(w[22]) + w[21];
    w[38] = sigma1(w[36]) + w[31] + sigma0(w[23]) + w[22];
    w[39] = sigma1(w[37]) + w[32] + sigma0(w[24]) + w[23];
    w[40] = sigma1(w[38]) + w[33] + sigma0(w[25]) + w[24];
    w[41] = sigma1(w[39]) + w[34] + sigma0(w[26]) + w[25];
    w[42] = sigma1(w[40]) + w[35] + sigma0(w[27]) + w[26];
    w[43] = sigma1(w[41]) + w[36] + sigma0(w[28]) + w[27];
    w[44] = sigma1(w[42]) + w[37] + sigma0(w[29]) + w[28];
    w[45] = sigma1(w[43]) + w[38] + sigma0(w[30]) + w[29];
    w[46] = sigma1(w[44]) + w[39] + sigma0(w[31]) + w[30];
    w[47] = sigma1(w[45]) + w[40] + sigma0(w[32]) + w[31];
    w[48] = sigma1(w[46]) + w[41] + sigma0(w[33]) + w[32];
    w[49] = sigma1(w[47]) + w[42] + sigma0(w[34]) + w[33];
    w[50] = sigma1(w[48]) + w[43] + sigma0(w[35]) + w[34];
    w[51] = sigma1(w[49]) + w[44] + sigma0(w[36]) + w[35];
    w[52] = sigma1(w[50]) + w[45] + sigma0(w[37]) + w[36];
    w[53] = sigma1(w[51]) + w[46] + sigma0(w[38]) + w[37];
    w[54] = sigma1(w[52]) + w[47] + sigma0(w[39]) + w[38];
    w[55] = sigma1(w[53]) + w[48] + sigma0(w[40]) + w[39];
    w[56] = sigma1(w[54]) + w[49] + sigma0(w[41]) + w[40];
    w[57] = sigma1(w[55]) + w[50] + sigma0(w[42]) + w[41];
    w[58] = sigma1(w[56]) + w[51] + sigma0(w[43]) + w[42];
    w[59] = sigma1(w[57]) + w[52] + sigma0(w[44]) + w[43];
    w[60] = sigma1(w[58]) + w[53] + sigma0(w[45]) + w[44];
    w[61] = sigma1(w[59]) + w[54] + sigma0(w[46]) + w[45];
    w[62] = sigma1(w[60]) + w[55] + sigma0(w[47]) + w[46];
    w[63] = sigma1(w[61]) + w[56] + sigma0(w[48]) + w[47];

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    SHA256_ROUND(0,  a, b, c, d, e, f, g, h, tmp1);
    SHA256_ROUND(1,  h, a, b, c, d, e, f, g, tmp1);
    SHA256_ROUND(2,  g, h, a, b, c, d, e, f, tmp1);
    SHA256_ROUND(3,  f, g, h, a, b, c, d, e, tmp1);
    SHA256_ROUND(4,  e, f, g, h, a, b, c, d, tmp1);
    SHA256_ROUND(5,  d, e, f, g, h, a, b, c, tmp1);
    SHA256_ROUND(6,  c, d, e, f, g, h, a, b, tmp1);
    SHA256_ROUND(7,  b, c, d, e, f, g, h, a, tmp1);

    SHA256_ROUND(8,  a, b, c, d, e, f, g, h, tmp1);
    SHA256_ROUND(9,  h, a, b, c, d, e, f, g, tmp1);
    SHA256_ROUND(10, g, h, a, b, c, d, e, f, tmp1);
    SHA256_ROUND(11, f, g, h, a, b, c, d, e, tmp1);
    SHA256_ROUND(12, e, f, g, h, a, b, c, d, tmp1);
    SHA256_ROUND(13, d, e, f, g, h, a, b, c, tmp1);
    SHA256_ROUND(14, c, d, e, f, g, h, a, b, tmp1);
    SHA256_ROUND(15, b, c, d, e, f, g, h, a, tmp1);

    SHA256_ROUND(16, a, b, c, d, e, f, g, h, tmp1);
    SHA256_ROUND(17, h, a, b, c, d, e, f, g, tmp1);
    SHA256_ROUND(18, g, h, a, b, c, d, e, f, tmp1);
    SHA256_ROUND(19, f, g, h, a, b, c, d, e, tmp1);
    SHA256_ROUND(20, e, f, g, h, a, b, c, d, tmp1);
    SHA256_ROUND(21, d, e, f, g, h, a, b, c, tmp1);
    SHA256_ROUND(22, c, d, e, f, g, h, a, b, tmp1);
    SHA256_ROUND(23, b, c, d, e, f, g, h, a, tmp1);

    SHA256_ROUND(24, a, b, c, d, e, f, g, h, tmp1);
    SHA256_ROUND(25, h, a, b, c, d, e, f, g, tmp1);
    SHA256_ROUND(26, g, h, a, b, c, d, e, f, tmp1);
    SHA256_ROUND(27, f, g, h, a, b, c, d, e, tmp1);
    SHA256_ROUND(28, e, f, g, h, a, b, c, d, tmp1);
    SHA256_ROUND(29, d, e, f, g, h, a, b, c, tmp1);
    SHA256_ROUND(30, c, d, e, f, g, h, a, b, tmp1);
    SHA256_ROUND(31, b, c, d, e, f, g, h, a, tmp1);

    SHA256_ROUND(32, a, b, c, d, e, f, g, h, tmp1);
    SHA256_ROUND(33, h, a, b, c, d, e, f, g, tmp1);
    SHA256_ROUND(34, g, h, a, b, c, d, e, f, tmp1);
    SHA256_ROUND(35, f, g, h, a, b, c, d, e, tmp1);
    SHA256_ROUND(36, e, f, g, h, a, b, c, d, tmp1);
    SHA256_ROUND(37, d, e, f, g, h, a, b, c, tmp1);
    SHA256_ROUND(38, c, d, e, f, g, h, a, b, tmp1);
    SHA256_ROUND(39, b, c, d, e, f, g, h, a, tmp1);

    SHA256_ROUND(40, a, b, c, d, e, f, g, h, tmp1);
    SHA256_ROUND(41, h, a, b, c, d, e, f, g, tmp1);
    SHA256_ROUND(42, g, h, a, b, c, d, e, f, tmp1);
    SHA256_ROUND(43, f, g, h, a, b, c, d, e, tmp1);
    SHA256_ROUND(44, e, f, g, h, a, b, c, d, tmp1);
    SHA256_ROUND(45, d, e, f, g, h, a, b, c, tmp1);
    SHA256_ROUND(46, c, d, e, f, g, h, a, b, tmp1);
    SHA256_ROUND(47, b, c, d, e, f, g, h, a, tmp1);

    SHA256_ROUND(48, a, b, c, d, e, f, g, h, tmp1);
    SHA256_ROUND(49, h, a, b, c, d, e, f, g, tmp1);
    SHA256_ROUND(50, g, h, a, b, c, d, e, f, tmp1);
    SHA256_ROUND(51, f, g, h, a, b, c, d, e, tmp1);
    SHA256_ROUND(52, e, f, g, h, a, b, c, d, tmp1);
    SHA256_ROUND(53, d, e, f, g, h, a, b, c, tmp1);
    SHA256_ROUND(54, c, d, e, f, g, h, a, b, tmp1);
    SHA256_ROUND(55, b, c, d, e, f, g, h, a, tmp1);

    SHA256_ROUND(56, a, b, c, d, e, f, g, h, tmp1);
    SHA256_ROUND(57, h, a, b, c, d, e, f, g, tmp1);
    SHA256_ROUND(58, g, h, a, b, c, d, e, f, tmp1);
    SHA256_ROUND(59, f, g, h, a, b, c, d, e, tmp1);
    SHA256_ROUND(60, e, f, g, h, a, b, c, d, tmp1);
    SHA256_ROUND(61, d, e, f, g, h, a, b, c, tmp1);
    SHA256_ROUND(62, c, d, e, f, g, h, a, b, tmp1);
    SHA256_ROUND(63, b, c, d, e, f, g, h, a, tmp1);

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
    ctx->size = (unsigned long)0;

    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

void sha256_update(sha256_context *ctx, const unsigned char *input, size_t input_len) {
    unsigned long offset = ctx->size % 64;
    ctx->size += (unsigned long)input_len;

    for (size_t i = 0; i < input_len; i++) {
        ctx->input[offset++] = input[i];

        if (offset == 64) {
            sha256_process(ctx, ctx->input);
            offset = 0;
        }
    }
}

void sha256_final(sha256_context *ctx, unsigned char *output) {
    unsigned long offset = ctx->size % 64;
    ctx->input[offset++] = (unsigned char)0x80;

    if (offset > 56) {
        while (offset < 64) {
            ctx->input[offset++] = (unsigned char)0x00;
        }

        sha256_process(ctx, ctx->input);
        offset = 0;
    }

    while (offset != 56) {
        ctx->input[offset++] = (unsigned char)0x00;
    }

    unsigned long len_bits = ctx->size * 8;
    ctx->input[56] = (unsigned char)(len_bits >> 56);
    ctx->input[57] = (unsigned char)(len_bits >> 48);
    ctx->input[58] = (unsigned char)(len_bits >> 40);
    ctx->input[59] = (unsigned char)(len_bits >> 32);
    ctx->input[60] = (unsigned char)(len_bits >> 24);
    ctx->input[61] = (unsigned char)(len_bits >> 16);
    ctx->input[62] = (unsigned char)(len_bits >> 8);
    ctx->input[63] = (unsigned char)(len_bits);

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

void compute_sha256(const unsigned char *input, const unsigned int input_len,
                 __private unsigned char *output) {
    sha256_context ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, input, input_len);
    sha256_final(&ctx, output);
}
