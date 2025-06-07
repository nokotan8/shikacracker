#define MAX_PWD_LEN 32 // Change this if you need longer pwds

__kernel void generate_from_mask(
    __constant const char *charset_basis,           // flattened char **
    __constant const unsigned int *charset_offsets, // length = pwd_length
    __constant const unsigned int *charset_lengths, // length = pwd_length
    const unsigned int pwd_length, __global unsigned char *output,
    const unsigned int block_size) {
    size_t gid = get_global_id(0);
    size_t idx = gid;

    size_t curr_idx = idx;
    char pwd_candidate[MAX_PWD_LEN];

    for (int pos = (int)pwd_length - 1; pos >= 0; pos--) {
        size_t len = charset_lengths[pos]; // # of items in charset_basis[pos] (if not flattened)
        size_t char_idx = curr_idx % len;  // index within charset_basis[pos]
        curr_idx /= len;
        pwd_candidate[pos] = charset_basis[charset_offsets[pos] + char_idx];
    }

    size_t output_offset = (gid % block_size) * 16; // hash length = 16
    compute_md5((unsigned char *)pwd_candidate, pwd_length,
                output + output_offset);
}
