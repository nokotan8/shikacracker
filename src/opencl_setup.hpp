#ifndef INC_OPENCL_SETUP_H
#define INC_OPENCL_SETUP_H

#define CL_HPP_ENABLE_EXCEPTIONS

#include <filesystem>
#include <string>

/**
 * From https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
 */
std::string get_file_contents(const std::filesystem::path filename);

int opencl_setup();

#endif // INC_OPENCL_SETUP_H
