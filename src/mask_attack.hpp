#ifndef INC_MASK_ATTACK_H
#define INC_MASK_ATTACK_H

#include "concurrent_set.hpp"
#include <string>

void mask_attack(const std::string &mask,
                 concurrent_set<std::string> &input_hashes);

#endif // INC_MASK_ATTACK_H
