/**
* Compute SHA256 hashes from a given mask.
*
* Dependencies:
*   - sha256.cl
*   - murmurhash3.cl
*   - bitops.cl
*/

#define MAX_PWD_LEN 128 // Change this if you need longer pwds
#define SHA256_DIGEST_LEN 32

__kernel void generate_from_mask_sha256(
    __constant const unsigned char *charset_basis,          // 0. flattened char **
    __constant const unsigned int *charset_offsets,         // 1. length = pwd_length
    __constant const unsigned int *charset_lengths,         // 2. length = pwd_length
    __constant const unsigned char *pwd_first_half,         // 3. part of the mask that is already generated
    const unsigned int curr_length,                         // 4. length of pwd_first_half
    const unsigned int pwd_length,                          // 5. length of each password candidate
    __constant const short *bucket_status,                  // 6. Whether each bucket in the host hash map is empty
    const unsigned int num_buckets,                         // 7. length of bucket_status
    __global char *output,                                  // 8. array to store hash output
    __global bool *output_status,                           // 9. whether the hash of the digest matches with an occupied bucket
    __global unsigned char *output_reverse,                 // 10. array of hash reverses
    const unsigned int block_size                           // 11. used to calculate offset into 8. and 9.
) {

    size_t gid = get_global_id(0);
    size_t idx = gid;

    size_t curr_idx = idx;
    unsigned char pwd_candidate[MAX_PWD_LEN];

    size_t output_status_offset = gid % block_size;
    size_t output_offset =  output_status_offset * SHA256_DIGEST_LEN * 2; // hash length = 16, double for hex
    size_t output_reverse_offset = output_status_offset * pwd_length;

    for (int pos = (int)pwd_length - 1; pos >= (int)curr_length; pos--) {
        size_t len = charset_lengths[pos]; // # of items in charset_basis[pos] (if not flattened)
        size_t char_idx = curr_idx % len;  // index within charset_basis[pos]
        curr_idx /= len;
        pwd_candidate[pos] = charset_basis[charset_offsets[pos] + char_idx];
        output_reverse[output_reverse_offset + pos] = pwd_candidate[pos];
    }

    for (size_t pos = 0; pos < curr_length; pos++) {
        pwd_candidate[pos] = pwd_first_half[pos];
        output_reverse[output_reverse_offset + pos] = pwd_candidate[pos];
    }

    unsigned char output_raw[SHA256_DIGEST_LEN];
    compute_sha256(pwd_candidate, pwd_length, output_raw);

    char output_hex[SHA256_DIGEST_LEN * 2];
    char_to_hex(output_raw, output_hex, (unsigned int)SHA256_DIGEST_LEN);

    unsigned int key;
    murmurhash3_32(output_hex, SHA256_DIGEST_LEN * 2, MURMURHASH_SEED, &key);

    output_status[output_status_offset] = bucket_status[key % num_buckets];

    for (size_t i = 0; i < SHA256_DIGEST_LEN * 2; i++) {
        output[i + output_offset] = output_hex[i];
    }
}

