/* Charsets that are used during mask attacks, etc. */
#pragma once
#include <string>

// Default charsets. Equivalent to those found in hashcat:
// https://hashcat.net/wiki/doku.php?id=mask_attack
// except for ALPHANUM, which was added for convenience.
const std::string NO_CHARS = "";                                   // error
const std::string ALPHA_LOWER = "abcdefghijklmnopqrstuvwxyz";      // ?l
const std::string ALPHA_UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";      // ?u
const std::string DIGITS = "0123456789";                           // ?d
const std::string SYMBOLS = " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"; // ?s
const std::string HEX_LOWER = "0123456789abcdef";                  // ?h
const std::string HEX_UPPER = "0123456789ABCDEF";                  // ?H
const std::string ALPHANUM = ALPHA_LOWER + ALPHA_LOWER + DIGITS;   // ?n
const std::string ALL_CHARS =
    ALPHA_LOWER + ALPHA_UPPER + DIGITS + SYMBOLS; // ?A

// User-defined charsets
extern std::string CUSTOM_CHARSET1;
extern std::string CUSTOM_CHARSET2;
extern std::string CUSTOM_CHARSET3;
extern std::string CUSTOM_CHARSET4;

// Converts a charset identifier, i.e. the char
// inputted by the user after '?', to a list of
// all characters in that charset in std::string format.
const std::string &get_charset(char identifier);
