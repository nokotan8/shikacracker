#include "mask_attack.hpp"
#include "charsets.hpp"
#include "globals.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Converts a charset identifier, i.e. the char
// inputted by the user after '?', to a list of
// all characters in that charset in std::string format.
const std::string &get_charset(char identifier) {
    switch (identifier) {
        case 'l':
            return ALPHA_LOWER;
        case 'u':
            return ALPHA_UPPER;
        case 'd':
            return DIGITS;
        case 's':
            return SYMBOLS;
        case 'h':
            return HEX_LOWER;
        case 'H':
            return HEX_UPPER;
        case 'n':
            return ALPHANUM;
        case 'a':
            return ALL_CHARS;
        default:
            return NO_CHARS;
    }
}

// Converts user-inputted mask string into a vector
// of strings, where each string represents the possible
// character(s) at each position.
const std::vector<std::string> parse_mask() {
    std::vector<std::string> res;
    for (long unsigned int i = 0; i < mask.size(); i++) {
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
                std::string charset = get_charset(mask[i + 1]);
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

// Perform a mask attack, similar to:
// https://hashcat.net/wiki/doku.php?id=mask_attack
void mask_attack(concurrent_set<std::string> &input_hashes) {
    std::vector<std::string> mask_format = parse_mask();
    for (auto &s : mask_format) {
        for (char c : s)
            std::cout << c << ' ';
        std::cout << '\n';
    }
}
