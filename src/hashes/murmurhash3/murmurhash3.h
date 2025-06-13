/**
 * MurmurHash3 non-cryptographic hash function. Modified from:
 * https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp
 * MurmurHash is released under the MIT license.
 *
 * Tweaked to be valid within an OpenCL kernel, where memcpy is not available.
 * Only works correctly on little-endian systems.
 */

#ifndef INC_MURMURHASH3_H
#define INC_MURMURHASH3_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

void murmurhash3_32(const void *key, size_t len, uint32_t seed, void *output);

#ifdef __cplusplus
}
#endif

#endif // INC_MURMURHASH3_H
