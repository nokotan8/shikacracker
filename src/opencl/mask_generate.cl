#define MAX_PWD_LEN 128 // Change this if you need longer pwds
#define MD5_DIGEST_LEN 16

__kernel void generate_from_mask_md5(
    __constant const unsigned char *charset_basis,          // flattened char **
    __constant const unsigned int *charset_offsets,         // length = pwd_length
    __constant const unsigned int *charset_lengths,         // length = pwd_length
    __constant const unsigned char *pwd_first_half,         // part of the mask that is already generated
    const unsigned int curr_length,                         // length of pwd_first_half
    const unsigned int pwd_length,
    __global unsigned char *output,
    const unsigned int block_size) {

    size_t gid = get_global_id(0);
    size_t idx = gid;

    size_t curr_idx = idx;
    unsigned char pwd_candidate[MAX_PWD_LEN];

    for (int pos = (int)pwd_length - 1; pos >= (int)curr_length; pos--) {
        size_t len = charset_lengths[pos]; // # of items in charset_basis[pos] (if not flattened)
        size_t char_idx = curr_idx % len;  // index within charset_basis[pos]
        curr_idx /= len;
        pwd_candidate[pos] = charset_basis[charset_offsets[pos] + char_idx];
    }

    for (size_t pos = 0; pos < curr_length; pos++) {
        pwd_candidate[pos] = pwd_first_half[pos];
    }

    unsigned char output_raw[MD5_DIGEST_LEN];
    compute_md5(pwd_candidate, pwd_length, output_raw);

    size_t output_offset = (gid % block_size) * 32; // hash length = 16, double for hex
    char_to_hex(output_raw, output + output_offset, (unsigned int)MD5_DIGEST_LEN);
}

