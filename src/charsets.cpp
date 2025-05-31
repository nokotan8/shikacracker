#include "charsets.hpp"
#include <array>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_set>

std::string NO_CHARS = "";
std::string ALPHA_LOWER = "abcdefghijklmnopqrstuvwxyz";
std::string ALPHA_UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
std::string DIGITS = "0123456789";
std::string SYMBOLS = " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
std::string HEX_LOWER = "0123456789abcdef";
std::string HEX_UPPER = "0123456789ABCDEF";
std::string ALPHANUM = ALPHA_LOWER + ALPHA_UPPER + DIGITS;
std::string ALL_CHARS = ALPHA_LOWER + ALPHA_UPPER + DIGITS + SYMBOLS;

std::string CUSTOM_CHARSET1 = "";
std::string CUSTOM_CHARSET2 = "";
std::string CUSTOM_CHARSET3 = "";
std::string CUSTOM_CHARSET4 = "";

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

void order_charsets(std::filesystem::path freq_file) {
    std::ifstream input_file(freq_file);

    std::array<std::string *, 12> charstrings = {
        &ALPHA_LOWER,     &ALPHA_UPPER,     &DIGITS,          &SYMBOLS,
        &HEX_LOWER,       &HEX_UPPER,       &ALPHANUM,        &ALL_CHARS,
        &CUSTOM_CHARSET1, &CUSTOM_CHARSET2, &CUSTOM_CHARSET3, &CUSTOM_CHARSET4};

    std::array<std::unordered_set<char>, 12> charsets;

    for (size_t i = 0; i < charstrings.size(); i++) {
        for (char c : *charstrings[i]) {
            charsets[i].insert(c);
        }
    }

    for (auto &charset : charstrings) {
        *charset = "";
    }

    if (!input_file.is_open()) {
        throw std::invalid_argument("Specified frequency file does not exist");
    }
    std::string line;
    while (getline(input_file, line) && line.size() > 1) {
        int chr = line[0];
        for (size_t i = 0; i < charsets.size(); i++) {
            if (charsets[i].count(chr)) {
                charstrings[i]->push_back(chr);
                charsets[i].erase(chr);
            }
        }
    }
    input_file.close();

    for (size_t i = 0; i < charsets.size(); i++)
        for (auto it = charsets[i].begin(); it != charsets[i].end(); i++)
            charstrings[i]->push_back(*it);

    return;
}
