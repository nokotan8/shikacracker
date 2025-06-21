#ifndef INC_MODES_H
#define INC_MODES_H

#include <filesystem>
#include <string>

/**
 * From https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
 */
std::string get_file_contents(const std::filesystem::path filename);

std::string get_mask_kernel(int hash_type, std::string &kernel_fn_name,
                            size_t *digest_len);

#endif // INC_MODES_H
