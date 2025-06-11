#include "mask_attack.hpp"
#include "charsets.hpp"
#include "concurrent_set.hpp"
#include "entry_buffer.hpp"
#include "globals.hpp"
#include "opencl_setup.hpp"

#include <CL/opencl.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <new>
#include <stdexcept>
#include <string.h>
#include <string>
#include <thread>

size_t parse_mask(const std::string &mask, unsigned char **charset,
                  unsigned int **charset_offsets,
                  unsigned int **charset_lengths, unsigned int *pwd_length) {
    if (mask.empty()) {
        throw std::invalid_argument("mask must be at least 1 character long");
    } else if (pwd_length == NULL) {
        throw std::invalid_argument("pwd_length cannot be NULL");
    }

    if (*charset != nullptr || *charset_offsets != nullptr ||
        *charset_lengths != nullptr) {
        throw std::invalid_argument(
            "charset, charset_offsets and charset_lengths must be NULL");
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
    *charset = (unsigned char *)malloc(sizeof(unsigned char) * charset_len);
    if (*charset == nullptr) {
        throw std::bad_alloc();
    }
    *charset_offsets =
        (unsigned int *)malloc(sizeof(unsigned int) * (*pwd_length));
    if (*charset_offsets == nullptr) {
        free(*charset);
        *charset = nullptr;
        throw std::bad_alloc();
    }
    *charset_lengths =
        (unsigned int *)malloc(sizeof(unsigned int) * (*pwd_length));
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
    for (size_t i = 0; i < mask.size(); i++, cset_len_offset_idx++) {
        if (mask[i] == '?') {
            if (mask[i + 1] == '?') {
                (*charset)[cset_idx] = '?';
                (*charset_lengths)[cset_len_offset_idx] = 1;
                (*charset_offsets)[cset_len_offset_idx] = cset_offset;
                cset_offset++;
                cset_idx++;
                i++;
            } else {
                const std::string &curr_charset = get_charset(mask[i + 1]);
                memcpy((*charset + cset_idx), curr_charset.data(),
                       curr_charset.size());
                cset_idx += curr_charset.size();

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
            cset_idx++;
        }
    }

    return charset_len;
}

/**
 * Generate the first on_host_len characters of candidates.
 */
void generate_candidates(entry_buffer<std::string> &candidate_buffer,
                         const unsigned char *charset_basis,
                         const unsigned int *charset_offsets,
                         const unsigned int *charset_lengths,
                         const unsigned int on_host_length) {
    if (on_host_length > 0) {
        unsigned int on_host_space_size = 1;
        for (unsigned int i = 0; i < on_host_length; i++) {
            on_host_space_size *= charset_lengths[i];
        }

        std::string res(on_host_length, '\0');
        for (unsigned int i = 0; i < on_host_space_size; i++) {
            unsigned int idx = i;
            for (int pos = (int)on_host_length - 1; pos >= 0; pos--) {
                size_t len = charset_lengths[pos];
                size_t char_idx = idx % len;
                idx /= len;
                res[pos] = charset_basis[charset_offsets[pos] + char_idx];
            }

            // std::cout << res << '\n';
            candidate_buffer.add_item(res);
        }
    } else {
        candidate_buffer.add_item("");
    }

    candidate_buffer.finished_add();
}

/**
 * From the given charset_basis, reconstruct the last
 * [length] characters of the candidate string with the
 * given offset.
 */
std::string get_candidate(const unsigned char *charset_basis,
                          const unsigned int *charset_offsets,
                          const unsigned int *charset_lengths,
                          const unsigned int offset,
                          const unsigned int length) {

    std::string res(length, '\0');
    unsigned int idx = offset;
    for (int pos = (int)length - 1; pos >= 0; pos--) {
        size_t len = charset_lengths[pos];
        size_t char_idx = idx % len;
        idx /= len;
        res[pos] = charset_basis[charset_offsets[pos] + char_idx];
    }

    return res;
}

/**
 * Performs a mask attack, similar to:
 * https://hashcat.net/wiki/doku.php?id=mask_attack
 */
void mask_attack(const std::string &mask,
                 concurrent_set<std::string> &input_hashes,
                 std::string kernel_fn_name) {
    unsigned char *charset_basis = nullptr;
    cl_uint *charset_offsets = nullptr;
    cl_uint *charset_lengths = nullptr;
    cl_uint pwd_length;

    size_t charset_len;
    try {
        charset_len = parse_mask(mask, &charset_basis, &charset_offsets,
                                 &charset_lengths, &pwd_length);

    } catch (std::invalid_argument &err) {
        return;
    } catch (std::bad_alloc &err) {
        fprintf(stderr, "No memory in mask_attack");
        return;
    }

    cl_uint on_host_length = pwd_length / 2;

    // # of candidates that will be generated from each
    // half-computed mask
    size_t charset_space_size = 1;
    for (cl_uint i = on_host_length; i < pwd_length; i++) {
        charset_space_size *= charset_lengths[i];
    }

    // Stores half-generated candidates
    entry_buffer<std::string> candidate_buffer(10);
    std::thread gen_candidates_thread(
        generate_candidates, std::ref(candidate_buffer), charset_basis,
        charset_offsets, charset_lengths, on_host_length);

    cl_uint block_size = std::min(charset_space_size, (size_t)4096 * 64);
    size_t digest_size = 16;

    cl::Context context(cl::Device::getDefault());
    cl::CommandQueue queue(context, cl::Device::getDefault());

    std::string kernel_code =
        get_file_contents("../src/opencl/bitops.cl") + "\n" +
        get_file_contents("../src/opencl/md5.cl") + "\n" +
        get_file_contents("../src/opencl/mask_generate.cl");

    cl::Program program(context, kernel_code,
                        CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);

    try {
        program.build(cl::Device::getDefault());
    } catch (cl::BuildError &err) {
        fprintf(
            stderr, "Error building: %s\n",
            program
                .getBuildInfo<CL_PROGRAM_BUILD_LOG>({cl::Device::getDefault()})
                .c_str());

        free(charset_basis);
        free(charset_offsets);
        free(charset_lengths);

        return;
    }

    cl::Buffer charset_basis_d(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                               sizeof(cl_char) * charset_len, charset_basis);
    cl::Buffer charset_offsets_d(context,
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_uint) * pwd_length, charset_offsets);
    cl::Buffer charset_lengths_d(context,
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_uint) * pwd_length, charset_lengths);
    cl::Buffer pwd_first_half_d(context, CL_MEM_READ_WRITE,
                                sizeof(cl_char) * on_host_length);
    cl::Buffer output_d(context, CL_MEM_WRITE_ONLY,
                        sizeof(cl_char) * digest_size * block_size * 2);

    cl_char *output =
        (cl_char *)malloc(sizeof(cl_char) * digest_size * block_size * 2);
    if (output == nullptr) {
        free(charset_basis);
        free(charset_offsets);
        free(charset_lengths);
        throw std::bad_alloc();
    }

    try {
        cl::Kernel kernel(program, kernel_fn_name);
        kernel.setArg(0, charset_basis_d);
        kernel.setArg(1, charset_offsets_d);
        kernel.setArg(2, charset_lengths_d);
        kernel.setArg(4, on_host_length);
        kernel.setArg(5, pwd_length);
        kernel.setArg(6, output_d);
        kernel.setArg(7, block_size);

        cl::Event curr_event;
        bool first_block = true;
        size_t num_hashes_prev;
        size_t block_offset_prev;
        std::string input_str_prev;

        while (input_hashes.empty() == false) {
            std::optional<std::string> input_str =
                candidate_buffer.remove_item();
            if (input_str == std::nullopt)
                break;

            size_t block_offset = 0;
            size_t num_blocks =
                (charset_space_size + block_size - 1) / block_size;
            for (size_t i = 0; i < num_blocks;
                 i++, block_offset += block_size) {
                size_t block_end =
                    std::min(block_size + block_offset, charset_space_size);
                size_t num_hashes = block_end - block_offset;

                queue.enqueueWriteBuffer(pwd_first_half_d, CL_TRUE, 0,
                                         sizeof(cl_char) * on_host_length,
                                         input_str.value().data());
                kernel.setArg(3, pwd_first_half_d);

                if (first_block == false) {
                    curr_event.wait();

                    queue.enqueueReadBuffer(output_d, CL_TRUE, 0,
                                            sizeof(cl_char) * digest_size *
                                                num_hashes_prev * 2,
                                            output);
                }

                queue.enqueueNDRangeKernel(kernel, cl::NDRange(block_offset),
                                           cl::NDRange(block_end),
                                           cl::NullRange, nullptr, &curr_event);

                if (first_block == false) {
                    std::string digest_hex(digest_size * 2, '\0');
                    for (size_t i = 0; i < num_hashes_prev; i++) {
                        size_t offset = i * digest_size * 2;
                        memcpy(digest_hex.data(), output + offset,
                               digest_size * 2);

                        if (input_hashes.count(digest_hex)) {
                            input_hashes.erase(digest_hex);
                            std::string orig_string =
                                input_str_prev +
                                get_candidate(charset_basis, charset_offsets,
                                              charset_lengths,
                                              block_offset_prev + i,
                                              pwd_length - on_host_length);
                            fprintf(stdout, "Reverse of hash %s found: %s\n",
                                    digest_hex.c_str(), orig_string.c_str());
                        }
                    }
                } else {
                    first_block = false;
                }

                block_offset_prev = block_offset;
                num_hashes_prev = num_hashes;
                input_str_prev = input_str.value();
            }
        }

        // Last block
        curr_event.wait();
        queue.enqueueReadBuffer(
            output_d, CL_TRUE, 0,
            sizeof(cl_char) * digest_size * num_hashes_prev * 2, output);

        std::string digest_hex(digest_size * 2, '\0');
        for (size_t i = 0; i < num_hashes_prev; i++) {
            size_t offset = i * digest_size * 2;
            memcpy(digest_hex.data(), output + offset, digest_size * 2);

            if (input_hashes.count(digest_hex)) {
                input_hashes.erase(digest_hex);
                std::string orig_string =
                    input_str_prev +
                    get_candidate(charset_basis, charset_offsets,
                                  charset_lengths, block_offset_prev + i,
                                  pwd_length - on_host_length);
                fprintf(stdout, "Reverse of hash %s found: %s\n",
                        digest_hex.c_str(), orig_string.c_str());
            }
        }
    } catch (cl::Error &err) {
        fprintf(stderr, "error: %s\n", err.what());
    }

    candidate_buffer.finished_add();

    gen_candidates_thread.join();

    free(output);
    free(charset_basis);
    free(charset_offsets);
    free(charset_lengths);

    return;
}
