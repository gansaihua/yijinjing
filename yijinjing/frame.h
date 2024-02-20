#pragma once

#include <cstdint>
#include <string>

#include "common.h"

namespace yijinjing {

FORWARD_DECLARE_PTR(Frame)
FORWARD_DECLARE_PTR(Page)
FORWARD_DECLARE_PTR(Reader)
FORWARD_DECLARE_PTR(Writer)
FORWARD_DECLARE_PTR(Journal)

struct FrameHeader {
    volatile uint32_t length;   // total frame length (including header and data body)
    uint32_t header_length;     // header length
    int64_t gen_time;           // generate time of the frame data
    volatile int32_t msg_type;  // msg type of the data in frame
    uint32_t source;            // source of this frame
    uint32_t dest;              // dest of this frame
} __attribute__((packed));

class Frame : public Event {
public:
    ~Frame() override = default;

    bool has_data() const { return header_->length > 0 && header_->msg_type > 0; }
    uintptr_t address() const { return reinterpret_cast<uintptr_t>(header_); }
    uint32_t frame_length() const { return header_->length; }
    uint32_t header_length() const { return header_->header_length; }
    uint32_t data_length() const override { return frame_length() - header_length(); }
    int64_t gen_time() const override { return header_->gen_time; }
    int32_t msg_type() const override { return header_->msg_type; }
    uint32_t source() const override { return header_->source; }
    uint32_t dest() const override { return header_->dest; }

    template <typename T>
    size_t copy_data(const T &data) {
        size_t length = sizeof(T);
        std::memcpy(const_cast<void *>(data_address()), &data, length);
        return length;
    }

protected:
    const void *data_address() const override { return reinterpret_cast<void *>(address() + header_length()); }

private:
    Frame() = default;
    void set_address(uintptr_t address) { header_ = reinterpret_cast<FrameHeader *>(address); }
    void move_to_next() { set_address(address() + frame_length()); }
    void set_header_length() { header_->header_length = sizeof(FrameHeader); }
    void set_data_length(uint32_t length) { header_->length = header_length() + length; }
    void set_gen_time(int64_t gen_time) { header_->gen_time = gen_time; }
    void set_msg_type(int32_t msg_type) { header_->msg_type = msg_type; }
    void set_source(uint32_t source) { header_->source = source; }
    void set_dest(uint32_t dest) { header_->dest = dest; }

    FrameHeader *header_;  // address with type, will keep moving forward until change page

    friend class Journal;
    friend class Writer;
};

}  // namespace yijinjing
