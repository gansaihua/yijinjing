#include "page.h"

#include <fmt/format.h>

#include <filesystem>
#include <regex>
#include <utility>

#include "mmap.h"

namespace yijinjing {

std::vector<int> list_page_id(LocationPtr location, uint32_t dest_id) {
    std::vector<int> page_ids;

    std::smatch m;
    std::regex pattern(fmt::format("{:08x}.(\\d+).journal", dest_id));
    for (const auto &p : std::filesystem::directory_iterator{location->path}) {
        std::string f = p.path().filename();
        if (std::regex_match(f, m, pattern)) {
            int page_id = std::stoi(m[1].str());
            page_ids.insert(std::upper_bound(page_ids.begin(), page_ids.end(), page_id), page_id);
        }
    }

    return page_ids;
}

Page::Page(LocationPtr location, uint32_t dest_id, const int id, const size_t size, uintptr_t address)
    : location_(location), dest_id_(dest_id), page_id_(id), size_(size), header_(reinterpret_cast<PageHeader *>(address)) {
    assert(address > 0);
}

Page::~Page() {
    release_mmap_buffer(address(), size_);
}

void Page::set_last_frame_position(uint64_t position) {
    const_cast<PageHeader *>(header_)->last_frame_position = position;
}

PagePtr Page::load(LocationPtr location, uint32_t dest_id, int page_id, bool is_writing) {
    uint32_t page_size = location->journal_page_size;
    std::string path = get_page_path(location, dest_id, page_id);
    bool new_created = ensure_file_exists(path, page_size);

    uintptr_t address = load_mmap_buffer(path, page_size, is_writing);
    if (address < 0) throw yijinjing_error("unable to load page for " + path);

    auto header = reinterpret_cast<PageHeader *>(address);
    if (header->last_frame_position == 0) {
        header->page_header_length = sizeof(PageHeader);
        header->page_size = page_size;
        header->frame_header_length = sizeof(FrameHeader);
        header->last_frame_position = header->page_header_length;
    }

    if (header->page_header_length != sizeof(PageHeader)) throw yijinjing_error(fmt::format("header length mismatch for page {}, required {}, found {}", path, sizeof(PageHeader), +header->page_header_length));
    if (header->page_size != page_size) throw yijinjing_error(fmt::format("page size mismatch, required {}, found {}", page_size, +header->page_size));

    return std::shared_ptr<Page>(new Page(location, dest_id, page_id, page_size, address));
}

std::string Page::get_page_path(LocationPtr location, uint32_t dest_id, int id) {
    return fmt::format("{}/{:08x}.{}.journal", location->path, dest_id, id);
}

int Page::find_page_id(LocationPtr location, uint32_t dest_id, int64_t time) {
    auto page_ids = list_page_id(location, dest_id);
    if (page_ids.empty()) return 1;
    if (time == 0 || page_ids.size() == 1) return page_ids.front();

    for (int i = page_ids.size() - 1; i >= 0; --i) {
        auto page = Page::load(location, dest_id, page_ids[i], false);
        auto page_begin_time = page->begin_time();
        if (page_begin_time < time) return page_ids[i];
    }
    return page_ids.front();
}

}  // namespace yijinjing
