/* Charsets that are used during mask attacks, etc. */
#pragma once
#include <filesystem>
#include <string>

// Default charsets. Equivalent to those found in hashcat:
// https://hashcat.net/wiki/doku.php?id=mask_attack
// except for ALPHANUM, which was added for convenience.
extern std::string NO_CHARS;    // error
extern std::string ALPHA_LOWER; // ?l
extern std::string ALPHA_UPPER; // ?u
extern std::string DIGITS;      // ?d
extern std::string SYMBOLS;     // ?s
extern std::string HEX_LOWER;   // ?h
extern std::string HEX_UPPER;   // ?H
extern std::string ALPHANUM;    // ?n
extern std::string ALL_CHARS;   // ?a

// User-defined charsets
extern std::string CUSTOM_CHARSET1;
extern std::string CUSTOM_CHARSET2;
extern std::string CUSTOM_CHARSET3;
extern std::string CUSTOM_CHARSET4;

// Converts a charset identifier, i.e. the char
// inputted by the user after '?', to a list of
// all characters in that charset in std::string format.
const std::string &get_charset(char identifier);

void order_charsets(std::filesystem::path freq_file);
