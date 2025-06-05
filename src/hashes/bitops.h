/**
 * Contains functions and macros for bit operations
 * commonly used in hashing algorithms.
 */
#include <stddef.h>
#include <stdint.h>

/**
 * Rotate x left by n positions.
 */
uint32_t rotate_left32(const uint32_t x, const size_t n);
