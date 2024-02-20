#include "mmap.h"

#include <spdlog/spdlog.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <regex>

#include "common.h"

namespace yijinjing {

uintptr_t load_mmap_buffer(const std::string &path, size_t size, bool is_writing, bool lazy) {
    bool master = is_writing || !lazy;
    int fd = open(path.c_str(), (master ? O_RDWR : O_RDONLY) | O_CREAT, (mode_t)0600);
    if (fd < 0) throw yijinjing_error("failed to open file for page " + path);

    if (master) {
        if (lseek(fd, size - 1, SEEK_SET) == -1) {
            close(fd);
            throw yijinjing_error("failed to stretch for page " + path);
        }
        if (write(fd, "", 1) == -1) {
            close(fd);
            throw yijinjing_error("unable to write for page " + path);
        }
    }

    void *buffer = mmap(0, size, master ? (PROT_READ | PROT_WRITE) : PROT_READ, MAP_SHARED, fd, 0);

    if (buffer == MAP_FAILED) {
        close(fd);
        throw yijinjing_error("Error mapping file to buffer");
    }

    if (!lazy && madvise(buffer, size, MADV_RANDOM) != 0 && mlock(buffer, size) != 0) {
        munmap(buffer, size);
        close(fd);
        throw yijinjing_error("failed to lock memory for page " + path);
    }

    close(fd);

    return reinterpret_cast<uintptr_t>(buffer);
}

bool release_mmap_buffer(uintptr_t address, size_t size, bool lazy) {
    void *buffer = reinterpret_cast<void *>(address);
    if (!lazy && munlock(buffer, size) != 0) return false;
    if (munmap(buffer, size) != 0) return false;
    return true;
}

}  // namespace yijinjing