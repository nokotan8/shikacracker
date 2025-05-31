#include "mask_attack.hpp"
#include "charsets.hpp"
#include "concurrent_set.hpp"
#include "entry_buffer.hpp"
#include "globals.hpp"
#include "hash.hpp"
#include <algorithm>
#include <functional>
#include <openssl/evp.h>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

/**
 * Converts user-inputted mask string into a vector
 * of strings, where each string represents the possible
 * character(s) at each position.
 */
const std::vector<std::string> parse_mask() {
    std::vector<std::string> res;
    for (size_t i = 0; i < mask.size(); i++) {
        if (mask[i] == '\\') {
            if (i < mask.size() - 1) {
                if (mask[i + 1] == '?') {
                    res.push_back("?");
                    i++;
                } else if (mask[i + 1] == '\\') {
                    res.push_back("\\");
                    i++;
                } else {
                    throw std::invalid_argument(
                        "'" + mask.substr(i, 2) +
                        "' is not a valid escape sequence");
                }
            } else {
                throw std::invalid_argument(
                    "'\\' at end of mask does not escape anything");
            }
        } else if (mask[i] == '?') {
            if (i < mask.size() - 1) {
                const std::string &charset = get_charset(mask[i + 1]);
                if (charset.size() > 0) {
                    res.push_back(charset);
                    i++;
                } else {
                    throw std::invalid_argument(
                        "'" + mask.substr(i, 2) +
                        "' does not reference a valid charset");
                }
            } else {
                throw std::invalid_argument(
                    "'?' at end of mask does not reference any charset");
            }
        } else {
            res.push_back(std::string(1, mask[i]));
        }
    }

    return res;
}

void generate_pwd_candidates_cont(std::string &curr_str,
                                  const std::vector<std::string> &mask_format,
                                  entry_buffer<std::string> &buffer, size_t i,
                                  concurrent_set<std::string> &input_hashes) {
    const size_t n = mask_format.size();
    if (i == n) {
        if (input_hashes.empty())
            buffer.finished_add();
        else
            buffer.add_item(curr_str);
        return;
    }

    for (char c : mask_format[i]) {
        curr_str[i] = c;
        generate_pwd_candidates_cont(curr_str, mask_format, buffer, i + 1,
                                     input_hashes);
    }

    return;
}

void generate_pwd_candidates_thread(std::string curr_str,
                                    const std::vector<std::string> &mask_format,
                                    entry_buffer<std::string> &buffer, size_t i,
                                    size_t from_idx, size_t until_idx,
                                    concurrent_set<std::string> &input_hashes) {
    if (i >= mask_format.size())
        return;

    for (size_t j = from_idx; j <= until_idx; j++) {
        curr_str[i] = mask_format[i][j];
        generate_pwd_candidates_cont(curr_str, mask_format, buffer, i + 1,
                                     input_hashes);
    }
}

void generate_pwd_candidates(std::string curr_str,
                             const std::vector<std::string> &mask_format,
                             entry_buffer<std::string> &buffer,
                             concurrent_set<std::string> &input_hashes) {
    const size_t n = mask_format.size();
    size_t i = 0;
    for (; i < n && mask_format[i].size() < 2; i++) {
        curr_str[i] = mask_format[i][0];
    }

    if (i == n) {
        if (input_hashes.empty())
            buffer.finished_add();
        else
            buffer.add_item(curr_str);
        return;
    }

    size_t generate_num_threads =
        std::min((size_t)num_threads, mask_format[i].size());
    size_t thread_charspace = mask_format[i].size() / generate_num_threads;
    std::vector<std::thread> generate_threads(generate_num_threads);

    for (size_t j = 0; j < generate_num_threads; j++) {
        size_t from_idx = j * thread_charspace;
        size_t until_idx;
        if (j == generate_num_threads - 1)
            until_idx = mask_format[i].size() - 1;
        else
            until_idx = from_idx + thread_charspace - 1;

        generate_threads[j] = std::thread(
            generate_pwd_candidates_thread, curr_str, std::ref(mask_format),
            std::ref(buffer), i, from_idx, until_idx, std::ref(input_hashes));
    }

    for (auto &thread : generate_threads) {
        thread.join();
    }

    return;
}

/**
 * Performs a mask attack, similar to:
 * https://hashcat.net/wiki/doku.php?id=mask_attack
 */
void mask_attack(concurrent_set<std::string> &input_hashes) {
    std::vector<std::string> mask_format = parse_mask();

    std::vector<std::thread> cracker_threads(num_threads);
    entry_buffer<std::string> buffer(num_threads * 4);
    const EVP_MD *hash_type = hash_mode_to_EVP_MD();

    for (int i = 0; i < num_threads; i++) {
        cracker_threads[i] = std::thread(hash_thread, std::ref(buffer),
                                         std::ref(input_hashes), hash_type);
    }

    std::string curr_str(mask_format.size(), '\0');
    generate_pwd_candidates(curr_str, mask_format, buffer, input_hashes);
    buffer.finished_add();

    for (auto &thread : cracker_threads) {
        thread.join();
    }

    if (input_hashes.size() != 0) {
        fprintf(stdout, "Hashes not cracked:\n");
        for (const auto &hash : input_hashes.set) {
            fprintf(stdout, "%s\n", hash.c_str());
        }
    }

    return;
}
