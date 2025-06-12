#ifndef INC_MURMURHASH3_H
#define INC_MURMURHASH3_H

#include <stddef.h>
#include <stdint.h>

void murmurhash3_32(const void *key, size_t len, uint32_t seed, void *output);

#endif // INC_MURMURHASH3_H
