#define MAX_PWD_LEN 32 // If you need longer masks, change this

__kernel void generate_from_mask(__constant const char *charset,            // flattened char **
                                 __constant const int *charset_offsets,     // length = pwd_length
                                 __constant const int *charset_lengths,     // length = pwd_length
                                 const size_t pwd_length, __global char *output,
                                 const size_t pwds_per_thread) {
    size_t idx = get_global_id(0) * total_per_thread;

    for (size_t i = 0; i < pwds_per_thread; i++) {
        size_t curr_idx = idx + i;

        char pwd_candidate[MAX_PWD_LEN];

        for (int pos = (int)pwd_length - 1; pos >= 0; pos--) {
            size_t len = charset_lengths[pos];      // # of items in charset[pos] (if not flattened)
            size_t char_idx = curr_idx % len;       // index within charset[pos]
            curr_idx /= len;
            pwd_candidate[pos] = charset[charset_offsets[pos] + char_idx];
        }

        // Do something with pwd 
    }
}
