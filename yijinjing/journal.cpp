#include "journal.h"

#include "common.h"
#include "page.h"

namespace yijinjing {

Journal::Journal(LocationPtr location, uint32_t dest_id, bool is_writing)
    : location_(std::move(location)),
      dest_id_(dest_id),
      is_writing_(is_writing),
      frame_(std::shared_ptr<Frame>(new Frame())),
      page_frame_nb_(0) {}

Journal::~Journal() {
    if (current_page_) current_page_.reset();
}

void Journal::next() {
    assert(current_page_);
    if (frame_->msg_type() == MTPageEnd) {
        load_next_page();
    } else {
        frame_->move_to_next();
        ++page_frame_nb_;
    }
}

void Journal::seek_to_time(int64_t nanotime) {
    int page_id = Page::find_page_id(location_, dest_id_, nanotime);
    load_page(page_id);

    while (current_page_->is_full() && current_page_->end_time() <= nanotime) load_next_page();
    while (frame_->has_data() && frame_->gen_time() <= nanotime) next();
}

void Journal::load_page(int page_id) {
    if (!current_page_ || current_page_->get_page_id() != page_id) {
        current_page_ = Page::load(location_, dest_id_, page_id, is_writing_);
        frame_->set_address(current_page_->first_frame_address());
        page_frame_nb_ = 0;
    }
}

void Journal::load_next_page() {
    load_page(current_page_->get_page_id() + 1);
}
}  // namespace yijinjing
