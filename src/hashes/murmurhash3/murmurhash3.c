#include "murmurhash3.h"
#include "../bitops.h"
#include <stdint.h>
#include <string.h>

static inline uint32_t fmix_32(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

void murmurhash3_32(const void *key, size_t len, uint32_t seed, void *output) {
    const uint8_t *data = (const uint8_t *)key;
    const size_t nblocks = len / 4;

    uint32_t h1 = seed;

    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    // body
    for (size_t i = 0; i < nblocks; i++) {
        uint32_t k1 = 0;

        // this is only equivalent to the below
        // memcpy on little-endian systems
        for (size_t j = 0; j < 4; j++) {
            k1 |= (data[i * 4 + j] << (8 * j));
        }
        // memcpy(&k1, data + i * 4, 4);

        k1 *= c1;
        k1 = rotate_left32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = rotate_left32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    // tail
    uint32_t k1 = 0;
    const uint8_t *tail = (const uint8_t *)(data + nblocks * 4);

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

    *(uint32_t *)output = h1;
}
