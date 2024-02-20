#include <iostream>

#include "reader.h"

using namespace yijinjing;

enum {
    MTQuote = 101,
    MTOrder = 102,
};

struct Quote {
    char symbol[16];
    double price;
    int volume;
};

struct Order {
    char symbol[16];
    double price;
    int volume;
};

int main() {
    auto home = std::make_shared<Location>(".");
    auto reader = std::make_shared<Reader>(false);
    reader->join(home, 20240221, 0);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    while (reader->data_available()) {
        auto frame = reader->current_frame();
        if (frame->msg_type() == MTQuote) {
            auto &data = frame->data<Quote>();
            std::cout << data.symbol << ',' << data.price << ',' << data.volume << std::endl;
        } else if (frame->msg_type() == MTOrder) {
            auto &data = frame->data<Order>();
            std::cout << data.symbol << ',' << data.price << ',' << data.volume << std::endl;
        }

        // std::cout << frame->msg_type() << ','
        //           << frame->has_data() << ','
        //           << frame->address() << ','
        //           << frame->frame_length() << ','
        //           << frame->header_length() << ','
        //           << frame->data_length() << ','
        //           << fmt::format("{:08x}", frame->source()) << ','
        //           << fmt::format("{:08x}", frame->dest()) << ','
        //           << Time::strftime(frame->gen_time()) << '\n';

        reader->next();
    }

    return 0;
}
