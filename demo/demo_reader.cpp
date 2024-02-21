#include <iostream>

#include "reader.h"

using namespace yijinjing;

int main() {
    auto home = std::make_shared<Location>(".");
    auto reader = std::make_shared<Reader>();
    reader->join(home, 20240221, 0);

    while (reader->data_available()) {
        auto frame = reader->current_frame();
        std::cout << frame->msg_type() << ','
                  << frame->has_data() << ','
                  << frame->address() << ','
                  << frame->frame_length() << ','
                  << frame->header_length() << ','
                  << frame->data_length() << ','
                  << fmt::format("{:08x}", frame->source()) << ','
                  << fmt::format("{:08x}", frame->dest()) << ','
                  << Time::strftime(frame->gen_time()) << '\n';
        reader->next();
    }

    return 0;
}
