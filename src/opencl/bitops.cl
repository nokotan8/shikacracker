unsigned int rotate_left32(const unsigned int x, const size_t n) {
    return (x << n) | (x >> (32 - n));
}

void char_to_hex(__private unsigned char *input, __global unsigned char *output,
                 const unsigned int size) {

    for (unsigned int i = 0; i < size; i++) {
        unsigned char byte = input[i];

        unsigned char lo = byte & 0x0F;
        unsigned char hi = (byte >> 4) & 0x0F;

        unsigned char hi_char = (hi < 10) ? ('0' + hi) : ('a' + hi - 10);
        unsigned char lo_char = (lo < 10) ? ('0' + lo) : ('a' + lo - 10);

        output[2 * i] = hi_char;
        output[2 * i + 1] = lo_char;
    }
}
