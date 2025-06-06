#include "mask_attack.hpp"
#include "charsets.hpp"
#include "concurrent_set.hpp"
#include "entry_buffer.hpp"
#include "globals.hpp"
// #include "hash.hpp"

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <stdexcept>
#include <string>
#include <vector>

size_t parse_mask(const std::string &mask, char **charset,
                  size_t **charset_offsets, size_t **charset_lengths,
                  size_t *pwd_length) {
    if (mask.empty()) {
        throw std::invalid_argument("mask must be at least 1 character long");
    } else if (pwd_length == NULL) {
        throw std::invalid_argument("pwd_length cannot be NULL");
    }

    if (*charset == nullptr) {
        free(*charset);
        *charset = nullptr;
    }
    if (*charset_offsets == nullptr) {
        free(*charset_offsets);
        *charset_offsets = nullptr;
    }
    if (*charset_lengths == nullptr) {
        free(*charset_lengths);
        *charset_lengths = nullptr;
    }

    // First get length of password and mask
    *pwd_length = 0;
    size_t charset_len = 0;
    for (size_t i = 0; i < mask.size(); i++) {
        if (mask[i] == '?') {
            if (i < mask.size() - 1) {
                if (mask[i + 1] == '?') {
                    (*pwd_length)++;
                    charset_len++;
                    i++;
                } else {
                    const std::string &curr_charset = get_charset(mask[i + 1]);
                    if (curr_charset.empty() == false) {
                        (*pwd_length)++;
                        charset_len += curr_charset.size();
                        i++;
                    } else {
                        throw std::invalid_argument(
                            "'" + mask.substr(i, 2) +
                            "' does not reference a valid charset");
                    }
                }
            } else {
                throw std::invalid_argument(
                    "'?' at end of mask does not reference any charset");
            }
        } else {
            (*pwd_length)++;
            charset_len++;
        }
    }

    // Every element *should* be assigned a value, so malloc over calloc
    *charset = (char *)malloc(sizeof(char) * charset_len);
    if (*charset == nullptr) {
        throw std::bad_alloc();
    }
    *charset_offsets = (size_t *)malloc(sizeof(size_t) * (*pwd_length));
    if (*charset_offsets == nullptr) {
        free(*charset);
        *charset = nullptr;
        throw std::bad_alloc();
    }
    *charset_lengths = (size_t *)malloc(sizeof(size_t) * (*pwd_length));
    if (*charset_lengths == nullptr) {
        free(*charset);
        *charset = nullptr;
        free(*charset_offsets);
        *charset_offsets = nullptr;
        throw std::bad_alloc();
    }

    // Then populate the arrays. Error checking already done.
    size_t cset_idx = 0;
    size_t cset_len_offset_idx = 0;
    size_t cset_offset = 0;
    for (size_t i = 0; i < mask.size();
         i++, cset_idx++, cset_len_offset_idx++) {
        if (mask[i] == '?') {
            if (mask[i + 1] == '?') {
                (*charset)[cset_idx] = '?';
                (*charset_lengths)[cset_len_offset_idx] = 1;
                (*charset_offsets)[cset_len_offset_idx] = cset_offset;
                cset_offset++;
                i++;
            } else {
                const std::string &curr_charset = get_charset(mask[i + 1]);
                for (char c : curr_charset) {
                    (*charset)[cset_idx] = c;
                    cset_idx++;
                }
                cset_idx--;

                (*charset_lengths)[cset_len_offset_idx] = curr_charset.size();
                (*charset_offsets)[cset_len_offset_idx] = cset_offset;
                cset_offset += curr_charset.size();
                i++;
            }
        } else {
            (*charset)[cset_idx] = mask[i];
            (*charset_lengths)[cset_len_offset_idx] = 1;
            (*charset_offsets)[cset_len_offset_idx] = cset_offset;
            cset_offset++;
        }
    }

    return charset_len;
}

/**
 * Generate password candidates based on the mask
 * provided in mask_format, starting at index i in
 * curr_str. Once the entire string is formed, add
 * it to buffer.
 */
void generate_pwd_candidates(std::string &curr_str,
                             std::vector<std::string> &mask_format,
                             entry_buffer<std::string> &buffer, int i,
                             concurrent_set<std::string> &input_hashes) {
    const int n = mask_format.size();
    if (i == n) {
        if (input_hashes.empty())
            buffer.finished_add();
        buffer.add_item(curr_str);
        return;
    }

    for (char c : mask_format[i]) {
        curr_str[i] = c;
        generate_pwd_candidates(curr_str, mask_format, buffer, i + 1,
                                input_hashes);
    }

    return;
}

/**
 * Performs a mask attack, similar to:
 * https://hashcat.net/wiki/doku.php?id=mask_attack
 */
void mask_attack(const std::string &mask,
                 concurrent_set<std::string> &input_hashes) {
    char *charset = nullptr;
    size_t *charset_offsets = nullptr;
    size_t *charset_lengths = nullptr;
    size_t pwd_length;

    size_t charset_len = parse_mask(mask, &charset, &charset_offsets,
                                    &charset_lengths, &pwd_length);

    /* For testing */
    // for (size_t i = 0; i < pwd_length; i++) {
    //     fprintf(stdout, "%zu ", charset_offsets[i]);
    // }
    // fprintf(stdout, "\n");
    //
    // for (size_t i = 0, curr_pos = 1; i < charset_len; i++) {
    //     if (curr_pos < pwd_length && i == charset_offsets[curr_pos]) {
    //         fprintf(stdout, "\n");
    //         curr_pos++;
    //     }
    //
    //     fprintf(stdout, "%c ", charset[i]);
    // }
    // fprintf(stdout, "\n");

    // std::vector<std::thread> cracker_threads(num_threads);
    // entry_buffer<std::string> buffer(num_threads * 4);
    // const EVP_MD *hash_type = hash_mode_to_EVP_MD();
    //
    // for (int i = 0; i < num_threads; i++) {
    //     cracker_threads[i] = std::thread(hash_thread, std::ref(buffer),
    //                                      std::ref(input_hashes), hash_type);
    // }
    //
    // std::string curr_str(mask_format.size(), '\0');
    // generate_pwd_candidates(curr_str, mask_format, buffer, 0, input_hashes);
    // buffer.finished_add();
    //
    // for (auto &thread : cracker_threads) {
    //     thread.join();
    // }

    // if (input_hashes.size() != 0) {
    //     fprintf(stdout, "Hashes not cracked:\n");
    //     for (const auto &hash : input_hashes.set) {
    //         fprintf(stdout, "%s\n", hash.c_str());
    //     }
    // }

    free(charset);
    free(charset_offsets);
    free(charset_lengths);

    return;
}
