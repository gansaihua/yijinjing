#include "reader.h"

#include <utility>

#include "page.h"

namespace yijinjing {

Reader::Reader() : current_(nullptr) {}

Reader::~Reader() { journals_.clear(); }

void Reader::join(LocationPtr location, uint32_t dest_id, const int64_t from_time) {
    for (const auto &journal : journals_) {
        if (journal->location_->uid == location->uid && journal->dest_id_ == dest_id) return;
    }
    journals_.push_back(std::make_shared<Journal>(location, dest_id, false));
    journals_.back()->seek_to_time(from_time);

    // do not sort if current_ is set (because we could be in process of reading)
    if (!current_) sort();
}

void Reader::disjoin(const uint32_t location_uid) {
    journals_.erase(std::remove_if(journals_.begin(), journals_.end(), [&](JournalPtr j) { return j->location_->uid == location_uid; }), journals_.end());
    current_ = nullptr;
    sort();
}

bool Reader::data_available() {
    sort();
    return current_ && current_frame()->has_data();
}

void Reader::seek_to_time(int64_t nanotime) {
    for (auto &journal : journals_) journal->seek_to_time(nanotime);
    sort();
}

void Reader::next() {
    if (current_) current_->next();
    sort();
}

void Reader::sort() {
    int64_t min_time = Time::now_in_nano();
    for (auto &journal : journals_) {
        auto &&frame = journal->current_frame();
        if (frame->has_data() && frame->gen_time() <= min_time) {
            min_time = frame->gen_time();
            current_ = journal.get();
        }
    }
}
}  // namespace yijinjing
