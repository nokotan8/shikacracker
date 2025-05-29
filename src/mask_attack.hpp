#pragma once
#include "concurrent_set.hpp"
#include <string>

void mask_attack(concurrent_set<std::string> &input_hashes);
