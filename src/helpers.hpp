#include <openssl/evp.h>
#include <string>

// Print general help, invoked with "shikacracker -h/--help"
void print_help_general();

// Remove leading and trailing whitespace from a string.
// Taken from https://stackoverflow.com/a/1798170
std::string trim_wspace(const std::string &str,
                        const std::string &whitespace = " \t");

// Convert hash_mode argument to EVP_MD *
// that can be passed into compute_hash
const EVP_MD *hash_mode_to_EVP_MD();
