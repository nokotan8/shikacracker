#define MAX_PWD_LEN 32 // Change this if you need longer pwds

__kernel void generate_from_mask(
    __constant const char *charset_basis,           // flattened char **
    __constant const unsigned int *charset_offsets, // length = pwd_length
    __constant const unsigned int *charset_lengths, // length = pwd_length
    __constant const char *pwd_first_half,          // part of the mask that is already generated
    const unsigned int curr_length,                 // length of pwd_first_half
    const unsigned int pwd_length,
    __global unsigned char *output,
    const unsigned int block_size) {
    size_t gid = get_global_id(0);
    size_t idx = gid;

    size_t curr_idx = idx;
    char pwd_candidate[MAX_PWD_LEN];

    for (int pos = (int)pwd_length - 1; pos >= (int)curr_length; pos--) {
        size_t len = charset_lengths[pos]; // # of items in charset_basis[pos] (if not flattened)
        size_t char_idx = curr_idx % len;  // index within charset_basis[pos]
        curr_idx /= len;
        pwd_candidate[pos] = charset_basis[charset_offsets[pos] + char_idx];
    }

    for (size_t pos = 0; pos < curr_length; pos++) {
        pwd_candidate[pos] = pwd_first_half[pos];
    }

    size_t output_offset = (gid % block_size) * 16; // hash length = 16
    compute_md5((unsigned char *)pwd_candidate, pwd_length, output + output_offset);
}

// __kernel void convert_to_hex(__global unsigned char* input, __global char* output, const unsigned int size) {
//     int id = get_global_id(0);
//     
//     if (id < size) {
//         unsigned char byte = input[id];
//         
//         // Extract high nibble (most significant 4 bits)
//         unsigned char high_nibble = (byte >> 4) & 0x0F;
//         
//         // Extract low nibble (least significant 4 bits)
//         unsigned char low_nibble = byte & 0x0F;
//         
//         // Map each nibble to its corresponding hexadecimal character
//         char high_char = (high_nibble < 10) ? ('0' + high_nibble) : ('a' + high_nibble - 10);
//         char low_char = (low_nibble < 10) ? ('0' + low_nibble) : ('a' + low_nibble - 10);
//         
//         // Write the two characters into the output buffer
//         output[2 * id] = high_char;
//         output[2 * id + 1] = low_char;
//     }
// }
