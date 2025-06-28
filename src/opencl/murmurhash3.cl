#define MURMURHASH_SEED 12345

static inline unsigned int fmix_32(unsigned int h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

void murmurhash3_32(const void *key, size_t len, unsigned int seed, void *output) {
    const unsigned char *data = (const unsigned char *)key;
    const size_t nblocks = len / 4;

    unsigned int h1 = seed;

    const unsigned int c1 = 0xcc9e2d51;
    const unsigned int c2 = 0x1b873593;

    // body
    for (size_t i = 0; i < nblocks; i++) {
        unsigned int k1 = 0;

        // this is only equivalent to the below
        // memcpy on little-endian systems
        k1 |= (data[i * 4]);
        k1 |= (data[i * 4 + 1] << (8));
        k1 |= (data[i * 4 + 2] << (16));
        k1 |= (data[i * 4 + 3] << (24));
        // memcpy(&k1, data + i * 4, 4);

        k1 *= c1;
        k1 = rotate_left32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = rotate_left32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    // tail
    unsigned int k1 = 0;
    const unsigned char *tail = (const unsigned char *)(data + nblocks * 4);

    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = rotate_left32(k1, 15);
            k1 *= c2;
            h1 ^= k1;
    };

    h1 ^= len;
    h1 = fmix_32(h1);

    *(unsigned int *)output = h1;
}
