#include "writer.h"

#include <mutex>
#include <utility>

#include "common.h"

namespace yijinjing {

constexpr uint32_t PAGE_ID_TRANC = 0xFFFF0000;
constexpr uint32_t FRAME_ID_TRANC = 0x0000FFFF;

Writer::Writer(LocationPtr location, uint32_t dest_id) : size_to_write_(0) {
    frame_id_base_ = location->uid ^ dest_id;
    frame_id_base_ = frame_id_base_ << 32;
    journal_ = std::make_shared<Journal>(location, dest_id, true);
    journal_->seek_to_time(Time::now_in_nano());
}

uint64_t Writer::current_frame_uid() {
    uint32_t page_part = (journal_->current_page_->page_id_ << 16) & PAGE_ID_TRANC;
    uint32_t frame_part = journal_->page_frame_nb_ & FRAME_ID_TRANC;
    return frame_id_base_ | (page_part | frame_part);
}

FramePtr Writer::open_frame(int32_t msg_type, uint32_t data_length) {
    assert(sizeof(FrameHeader) + data_length + sizeof(FrameHeader) <= journal_->current_page_->get_page_size());
    int64_t t = Time::now_in_nano();
    while (!writer_mtx_.try_lock()) {
        if (Time::now_in_nano() - t > TimeUnit::NANOSECONDS_PER_MILLISECOND) throw yijinjing_error("Can not lock writer for " + journal_->location_->path);
    }
    if (journal_->current_frame()->address() + sizeof(FrameHeader) + data_length > journal_->current_page_->address_border()) {
        close_page();
    }
    auto frame = journal_->current_frame();
    frame->set_header_length();
    frame->set_msg_type(msg_type);
    frame->set_source(journal_->location_->uid);
    frame->set_dest(journal_->dest_id_);
    return frame;
}

void Writer::close_frame(size_t data_length) {
    auto frame = journal_->current_frame();
    auto next_frame_address = frame->address() + frame->header_length() + data_length;
    assert(next_frame_address < journal_->current_page_->address_border());
    std::memset(reinterpret_cast<void *>(next_frame_address), 0, sizeof(FrameHeader));
    frame->set_gen_time(Time::now_in_nano());
    frame->set_data_length(data_length);
    journal_->current_page_->set_last_frame_position(frame->address() - journal_->current_page_->address());
    journal_->next();
    writer_mtx_.unlock();
}

void Writer::mark(int32_t msg_type) {
    open_frame(msg_type, 0);
    close_frame(0);
}

void Writer::mark_with_time(int64_t gen_time, int32_t msg_type) {
    assert(sizeof(FrameHeader) + sizeof(FrameHeader) <= journal_->current_page_->get_page_size());
    if (journal_->current_frame()->address() + sizeof(FrameHeader) > journal_->current_page_->address_border()) {
        mark(MTPageEnd);
        journal_->load_next_page();
    }
    auto frame = journal_->current_frame();
    frame->set_header_length();
    frame->set_msg_type(msg_type);
    frame->set_source(journal_->location_->uid);
    frame->set_dest(journal_->dest_id_);
    memset(reinterpret_cast<void *>(frame->address() + frame->header_length()), 0, sizeof(FrameHeader));
    frame->set_gen_time(gen_time);
    frame->set_data_length(0);
    journal_->current_page_->set_last_frame_position(frame->address() - journal_->current_page_->address());
    journal_->next();
}

void Writer::close_data() {
    size_t length = size_to_write_;
    size_to_write_ = 0;
    close_frame(length);
}

void Writer::write_raw(int32_t msg_type, uintptr_t data, uint32_t length) {
    auto frame = open_frame(msg_type, length);
    std::memcpy(const_cast<void *>(frame->data_address()), reinterpret_cast<void *>(data), length);
    close_frame(length);
}

template <>
void Writer::write(int32_t msg_type, const std::string &data) {
    write_raw(msg_type, reinterpret_cast<uintptr_t>(data.c_str()), data.length());
}

void Writer::close_page() {
    PagePtr last_page = journal_->current_page_;
    journal_->load_next_page();

    Frame last_page_frame;
    last_page_frame.set_address(last_page->last_frame_address());
    last_page_frame.move_to_next();
    last_page_frame.set_header_length();
    last_page_frame.set_msg_type(MTPageEnd);
    last_page_frame.set_gen_time(Time::now_in_nano());
    last_page_frame.set_data_length(0);
    last_page->set_last_frame_position(last_page_frame.address() - last_page->address());
}

}  // namespace yijinjing
