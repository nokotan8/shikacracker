#include "modes.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

std::string get_file_contents(const std::filesystem::path filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in) {
        std::ostringstream contents;
        contents << in.rdbuf();
        in.close();
        return (contents.str());
    } else {
        return "";
    }
}
std::string get_mask_kernel(int hash_type, std::string &kernel_fn_name,
                            size_t *digest_len) {
    switch (hash_type) {
        case 0: // MD5
            kernel_fn_name = "generate_from_mask_md5";
            *digest_len = 16;
            return get_file_contents("../src/opencl/bitops.cl") + "\n" +
                   get_file_contents("../src/opencl/md5.cl") + "\n" +
                   get_file_contents("../src/opencl/murmurhash3.cl") + "\n" +
                   get_file_contents("../src/opencl/mask_md5.cl");
        case 2: // SHA256
            kernel_fn_name = "generate_from_mask_sha256";
            *digest_len = 32;
            return get_file_contents("../src/opencl/bitops.cl") + "\n" +
                   get_file_contents("../src/opencl/sha256.cl") + "\n" +
                   get_file_contents("../src/opencl/murmurhash3.cl") + "\n" +
                   get_file_contents("../src/opencl/mask_sha256.cl");
    }

    return "";
}
