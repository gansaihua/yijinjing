#include "msg.h"
#include "writer.h"

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
    auto writer = std::make_shared<Writer>(home, 20240221, false);

    Quote q;
    strcpy(q.symbol, "ag2212");
    q.price = 123.4;
    q.volume = 10;

    // v1
    writer->write(MTQuote, q);

    // v2
    auto frame = writer->open_frame(MTQuote, sizeof(Quote));
    size_t i = frame->copy_data(q);
    writer->close_frame(i);

    // v3
    auto& o = writer->open_data<Order>(MTOrder);
    strcpy(o.symbol, "ag2312");
    o.price = 1234.5;
    o.volume = 101;
    writer->close_data();

    // // v4
    // writer->mark(MTPageEnd);

    return 0;
}
