#pragma once

#include <cassert>

#include "journal.h"

namespace kungfu {

class Writer {
public:
    Writer(LocationPtr location, uint32_t dest_id);

    LocationPtr get_location() const { return journal_->location_; }
    uint32_t get_dest() const { return journal_->dest_id_; }

    uint64_t current_frame_uid();

    FramePtr open_frame(int32_t msg_type, uint32_t length);
    void close_frame(size_t data_length);

    void mark(int32_t msg_type);
    void mark_with_time(int64_t gen_time, int32_t msg_type);

    template <typename T>
    T &open_data(int32_t msg_type) {
        auto frame = open_frame(msg_type, sizeof(T));
        size_to_write_ = sizeof(T);
        return const_cast<T &>(frame->data<T>());
    }

    void close_data();

    template <typename T>
    void write(int32_t msg_type, const T &data) {
        auto frame = open_frame(msg_type, sizeof(T));
        close_frame(frame->copy_data<T>(data));
    }

    template <typename T>
    void write_with_time(int64_t gen_time, int32_t msg_type, const T &data) {
        assert(sizeof(FrameHeader) + sizeof(T) + sizeof(FrameHeader) <= journal_->current_page_->get_page_size());
        if (journal_->current_frame()->address() + sizeof(FrameHeader) + sizeof(T) > journal_->current_page_->address_border()) {
            mark(MTPageEnd);
            journal_->load_next_page();
        }
        auto frame = journal_->current_frame();
        frame->set_header_length();
        frame->set_msg_type(msg_type);
        frame->set_source(journal_->location_->uid);
        frame->set_dest(journal_->dest_id_);

        frame->copy_data<T>(data);
        frame->set_gen_time(gen_time);
        frame->set_data_length(sizeof(T));
        journal_->current_page_->set_last_frame_position(frame->address() - journal_->current_page_->address());
        journal_->next();
    }

    void write_raw(int32_t msg_type, uintptr_t data, uint32_t length);

private:
    void close_page();

    std::mutex writer_mtx_;

    JournalPtr journal_;
    uint64_t frame_id_base_;

    size_t size_to_write_;
};

}  // namespace kungfu
