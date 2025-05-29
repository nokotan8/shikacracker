#include "charset.hpp"
#include <string>
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
