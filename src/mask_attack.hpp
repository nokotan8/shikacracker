#ifndef INC_MASK_ATTACK_H
#define INC_MASK_ATTACK_H

#include "concurrent_set.hpp"
#include <string>

void mask_attack(const std::string &mask,
                 concurrent_set<std::string> &input_hashes,
                 std::string kernel_fn_name);

#endif // INC_MASK_ATTACK_H
