#include <eventpp/eventdispatcher.h>

#include <chrono>
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

struct EventPolicies {
    static int getEvent(const EventPtr& e) { return e->msg_type(); }
};

int N = 1000000;

int main() {
    auto home = std::make_shared<Location>(".");
    auto reader = std::make_shared<Reader>();
    reader->join(home, 0, 0);

    eventpp::EventDispatcher<int, void(const EventPtr&), EventPolicies> dispatcher;
    dispatcher.appendListener(MTQuote, [](const EventPtr& e) {
        auto& data = e->data<Quote>();
        // std::cout << e->msg_type() << ':' << data.symbol << ',' << data.price << ',' << data.volume << std::endl;
    });
    dispatcher.appendListener(MTOrder, [](const EventPtr& e) {
        auto& data = e->data<Order>();
        // std::cout << e->msg_type() << ':' << data.symbol << ',' << data.price << ',' << data.volume << std::endl;
    });

    auto start = std::chrono::steady_clock::now();

    int i = 0;
    while (i < N) {
        while (reader->data_available()) {
            dispatcher.dispatch(reader->current_frame());
            reader->next();
            ++i;
        }
    }

    std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - start;
    std::cout << elapsed_seconds.count() << '\n';

    return 0;
}
