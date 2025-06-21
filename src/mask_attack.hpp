#ifndef INC_MASK_ATTACK_H
#define INC_MASK_ATTACK_H

#include "hash_map.hpp"
#include <string>

void mask_attack(const std::string &mask, hash_map<bool> &input_hashes,
                 size_t digest_len, const std::string &kernel_fn_name,
                 const std::string &kernel_code);

#endif // INC_MASK_ATTACK_H
