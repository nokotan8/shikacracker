/* Charsets that are used during mask attacks, etc. */
#pragma once
#include <string>

// Default charsets. Equivalent to those found in hashcat:
// https://hashcat.net/wiki/doku.php?id=mask_attack
// except for ALPHANUM, which was added for convenience.
const std::string ALPHA_LOWER = "abcdefghijklmnopqrstuvwxyz";      // ?l
const std::string ALPHA_UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";      // ?u
const std::string DIGITS = "0123456789";                           // ?d
const std::string SYMBOLS = " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"; // ?s
const std::string HEX_LOWER = "0123456789abcdef";                  // ?h
const std::string HEX_UPPER = "0123456789ABCDEF";                  // ?H
const std::string ALPHANUM = ALPHA_LOWER + ALPHA_LOWER + DIGITS;   // ?a
const std::string ALL_CHARS =
    ALPHA_LOWER + ALPHA_UPPER + DIGITS + SYMBOLS;                  // ?A

// User-defined charsets
extern std::string CUSTOM_CHARSET1;
extern std::string CUSTOM_CHARSET2;
extern std::string CUSTOM_CHARSET3;
extern std::string CUSTOM_CHARSET4;
