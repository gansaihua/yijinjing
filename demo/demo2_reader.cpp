#include <chrono>
#include <iostream>

#include "subject.h"

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

int N = 1000000;

int main() {
    auto home = std::make_shared<Location>(".");

    Observer observer;
    observer.observe(home, 0, 0);

    observer.subscribe(MTQuote, [](const EventPtr& e) {
        auto& data = e->data<Quote>();
        // std::cout << e->msg_type() << ':' << data.symbol << ',' << data.price << ',' << data.volume << std::endl;
    });
    observer.subscribe(MTOrder, [](const EventPtr& e) {
        auto& data = e->data<Order>();
        // std::cout << e->msg_type() << ':' << data.symbol << ',' << data.price << ',' << data.volume << std::endl;
    });

    auto start = std::chrono::steady_clock::now();

    observer.run();

    std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - start;
    std::cout << elapsed_seconds.count() << '\n';

    return 0;
}
