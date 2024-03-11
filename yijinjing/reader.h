#pragma once

#include <vector>

#include "journal.h"

namespace kungfu {

class Reader {
public:
    explicit Reader();

    ~Reader();

    /**
     * join journal at given data location
     * @param location where the journal locates
     * @param dest_id journal dest id
     * @param from_time subscribe events after this time, 0 means from start
     */
    void join(LocationPtr location, uint32_t dest_id, int64_t from_time);

    void disjoin(uint32_t location_uid);

    FramePtr current_frame() { return current_->current_frame(); }

    bool data_available();

    // seek journal to time
    void seek_to_time(int64_t nanotime);

    // seek next frame
    void next();

    void sort();

private:
    Journal *current_;

    std::vector<JournalPtr> journals_;
};

}  // namespace kungfu
