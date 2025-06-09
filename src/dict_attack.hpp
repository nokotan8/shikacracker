#ifndef INC_DICT_ATTACK_H
#define INC_DICT_ATTACK_H

#include "concurrent_set.hpp"
#include <string>

void dict_attack(concurrent_set<std::string> &hashes);

#endif // INC_DICT_ATTACK_H
