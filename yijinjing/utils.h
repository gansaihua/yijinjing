#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace kungfu {

uint32_t hash_str_32(const char *key, uint32_t len, uint32_t seed);

std::string expanduser(const std::string &path);

}  // namespace kungfu
