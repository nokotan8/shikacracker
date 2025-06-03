#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void MD5(const unsigned char *input, size_t n, unsigned char *output) {
    size_t total_len = n + 1; // Including the 1 bit
    while ((total_len % 64) != 56) {
        total_len++;
    }

    uint8_t *input_bits = calloc(total_len + 8, sizeof(uint8_t));
    if (input_bits == NULL) {
        fprintf(stderr, "No memory in MD5\n");
        exit(1);
    }

    memcpy(input_bits, input, n);

    // Append the 1 bit
    input_bits[n] = 0x80;

    uint64_t bit_len = (uint64_t)n * 8;
    memcpy(input_bits + total_len, &bit_len, 8);

    free(input_bits);
}
