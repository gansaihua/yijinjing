#pragma once

#include "common.h"
#include "frame.h"

namespace yijinjing {

struct PageHeader {
    uint32_t page_header_length;
    uint32_t page_size;
    uint32_t frame_header_length;
    uint64_t last_frame_position;
} __attribute__((packed));

class Page {
public:
    ~Page();

    uint32_t get_page_size() const { return header_->page_size; }
    LocationPtr get_location() const { return location_; }
    uint32_t get_dest_id() const { return dest_id_; }
    int get_page_id() const { return page_id_; }

    int64_t begin_time() const { return reinterpret_cast<FrameHeader*>(first_frame_address())->gen_time; }
    int64_t end_time() const { return reinterpret_cast<FrameHeader*>(last_frame_address())->gen_time; }

    uintptr_t address() const { return reinterpret_cast<uintptr_t>(header_); }
    uintptr_t address_border() const { return address() + header_->page_size - sizeof(FrameHeader); }

    uintptr_t first_frame_address() const { return address() + header_->page_header_length; }
    uintptr_t last_frame_address() const { return address() + header_->last_frame_position; }

    bool is_full() const { return last_frame_address() + reinterpret_cast<FrameHeader*>(last_frame_address())->length > address_border(); }

    static PagePtr load(LocationPtr location, uint32_t dest_id, int page_id, bool is_writing);
    static uint32_t get_page_size(LocationPtr location);
    static std::string get_page_path(LocationPtr location, uint32_t dest_id, int id);
    static int find_page_id(LocationPtr location, uint32_t dest_id, int64_t time);

private:
    Page(LocationPtr location, uint32_t dest_id, int page_id, size_t size, uintptr_t address);

    // update page header when new frame added
    void set_last_frame_position(uint64_t position);

    const LocationPtr location_;
    const uint32_t dest_id_;
    const int page_id_;
    const size_t size_;
    const PageHeader* header_;

    friend class Journal;
    friend class Writer;
    friend class Reader;
};

}  // namespace yijinjing
