#pragma once

#include <string>

namespace yijinjing {

// create a file of given size if not exists
bool ensure_file_exists(const std::string &path, size_t size);

uintptr_t load_mmap_buffer(const std::string &path, size_t size, bool is_writing = false);

bool release_mmap_buffer(uintptr_t address, size_t size);

}  // namespace yijinjing
