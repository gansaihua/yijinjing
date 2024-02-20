#pragma once

#include <mutex>
#include <utility>

#include "common.h"
#include "frame.h"
#include "msg.h"
#include "page.h"

namespace yijinjing {

// the abstraction of continuous memory access
class Journal {
public:
    Journal(LocationPtr location, uint32_t dest_id, bool is_writing, bool lazy);

    ~Journal();

    FramePtr current_frame() { return frame_; }

    // move current frame to the next available one
    void next();

    /**
     * makes sure after this call, next time user calls current_frame() gets the right available frame
     * (gen_time() > nanotime or writable)
     */
    void seek_to_time(int64_t nanotime);

private:
    void load_page(int page_id);

    // load next page, current page will be released if not empty
    void load_next_page();

    const LocationPtr location_;
    const uint32_t dest_id_;

    const bool is_writing_;
    const bool lazy_;

    PagePtr current_page_;
    FramePtr frame_;
    int page_frame_nb_;

    friend class Reader;
    friend class Writer;
};

}  // namespace yijinjing
