#pragma once

#include <string>

namespace yijinjing {

uintptr_t load_mmap_buffer(const std::string &path, size_t size, bool is_writing = false, bool lazy = true);

bool release_mmap_buffer(uintptr_t address, size_t size, bool lazy);

}  // namespace yijinjing
