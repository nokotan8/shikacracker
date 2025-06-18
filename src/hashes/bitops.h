/**
 * Contains functions and macros for bit operations
 * commonly used in hashing algorithms.
 */

#include <stddef.h>
#include <stdint.h>

#define rotate_left32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define rotate_right32(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
