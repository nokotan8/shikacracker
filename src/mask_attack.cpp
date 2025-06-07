#include "mask_attack.hpp"
#include "charsets.hpp"
#include "concurrent_set.hpp"
#include "entry_buffer.hpp"
#include "globals.hpp"
#include "opencl_setup.hpp"
// #include "hash.hpp"

#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <CL/opencl.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <new>
#include <stdexcept>
#include <string>
#include <vector>

size_t parse_mask(const std::string &mask, char **charset,
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
    *charset = (char *)malloc(sizeof(char) * charset_len);
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
    char *charset_basis = nullptr;
    unsigned int *charset_offsets = nullptr;
    unsigned int *charset_lengths = nullptr;
    unsigned int pwd_length;

    size_t charset_len;
    try {
        charset_len = parse_mask(mask, &charset_basis, &charset_offsets,
                                 &charset_lengths, &pwd_length);

    } catch (std::invalid_argument &err) {
        return;
    } catch (std::bad_alloc &err) {
        fprintf(stdout, "No memory in mask_attack");
        return;
    }

    // Total number of password candidates that will be generated
    size_t charset_space_size = 1;
    for (size_t i = 0; i < pwd_length; i++) {
        charset_space_size *= charset_lengths[i];
    }

    unsigned int block_size = 1024;
    size_t digest_size = 16;

    /* For testing */
    // for (size_t i = 0; i < pwd_length; i++) {
    //     fprintf(stdout, "%u ", charset_offsets[i]);
    // }
    // fprintf(stdout, "\n");
    //
    // for (size_t i = 0, curr_pos = 1; i < charset_len; i++) {
    //     if (curr_pos < pwd_length && i == charset_offsets[curr_pos]) {
    //         fprintf(stdout, "\n");
    //         curr_pos++;
    //     }
    //
    //     fprintf(stdout, "%c ", charset_basis[i]);
    // }
    // fprintf(stdout, "\n");

    cl::Context context(cl::Device::getDefault());
    cl::CommandQueue queue(context, cl::Device::getDefault());

    std::string kernel_code =
        get_file_contents("../src/opencl/md5.cl") + "\n" +
        get_file_contents("../src/opencl/mask_generate.cl");

    cl::Program program(context, kernel_code);
    try {
        program.build(cl::Device::getDefault());
    } catch (cl::BuildError &err) {
        std::cout << " Error building: "
                  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(
                         {cl::Device::getDefault()})
                  << "\n";
        exit(1);
    }

    cl::Buffer charset_basis_d(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                               sizeof(cl_char) * charset_len, charset_basis);
    cl::Buffer charset_offsets_d(context,
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_uint) * pwd_length, charset_offsets);
    cl::Buffer charset_lengths_d(context,
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_uint) * pwd_length, charset_lengths);
    cl::Buffer output_d(context, CL_MEM_WRITE_ONLY,
                        sizeof(cl_uchar) * digest_size * block_size);

    cl_uchar *output =
        (cl_uchar *)malloc(sizeof(cl_uchar) * digest_size * block_size);
    if (output == nullptr) {
        free(charset_basis);
        free(charset_offsets);
        free(charset_lengths);
        throw std::bad_alloc();
    }

    try {
        size_t block_offset = 0;
        size_t num_blocks = (charset_space_size + block_size - 1) / block_size;
        cl::Kernel kernel(program, "generate_from_mask");
        kernel.setArg(0, charset_basis_d);
        kernel.setArg(1, charset_offsets_d);
        kernel.setArg(2, charset_lengths_d);
        kernel.setArg(3, pwd_length);
        kernel.setArg(4, output_d);
        kernel.setArg(5, (cl_uint)block_size);

        for (size_t i = 0; i < num_blocks; i++, block_offset += block_size) {
            size_t block_end =
                std::min(block_size + block_offset, charset_space_size);
            size_t num_hashes = block_end - block_offset;
            // std::cout << block_offset << ' ' << block_end << '\n';

            queue.enqueueNDRangeKernel(kernel, cl::NDRange(block_offset),
                                       cl::NDRange(block_end));

            queue.enqueueReadBuffer(output_d, CL_TRUE, 0,
                                    sizeof(cl_uchar) * digest_size * num_hashes,
                                    output);

            std::string digest_hex(digest_size * 2 + 1, '\0');
            for (size_t i = 0; i < num_hashes; i++) {
                size_t offset = i * digest_size;
                for (size_t j = 0; j < digest_size; j++) {
                    snprintf(&digest_hex[j * 2], 3, "%02x", output[j + offset]);
                }

                if (input_hashes.count(digest_hex)) {
                    input_hashes.erase(digest_hex);
                    fprintf(
                        stdout,
                        "Reverse of hash %s found: WAIT I GOTTA IMPL THIS\n",
                        digest_hex.c_str());
                    size_t gid = block_offset + i;
                    std::cout << gid << '\n';
                }
            }
        }
    } catch (cl::Error &err) {
        std::cout << "error: " << err.what() << '\n';
    }

    free(output);
    free(charset_basis);
    free(charset_offsets);
    free(charset_lengths);

    return;
}
