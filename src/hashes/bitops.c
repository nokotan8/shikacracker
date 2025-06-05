#include <stddef.h>
#include <stdint.h>

uint32_t rotate_left32(const uint32_t x, const size_t n) {
    return (x << n) | (x >> (32 - n));
}
