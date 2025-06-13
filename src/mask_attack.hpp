#ifndef INC_MASK_ATTACK_H
#define INC_MASK_ATTACK_H

#include "hash_map.hpp"
#include <string>

void mask_attack(const std::string &mask, hash_map<bool> &input_hashes,
                 std::string kernel_fn_name);

#endif // INC_MASK_ATTACK_H
