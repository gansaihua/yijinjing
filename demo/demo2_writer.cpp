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
    auto home = std::make_shared<Location>(".", Category::STRATEGY);

    Publisher publisher(home);
    auto writer = publisher.get_writer(0);

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i != N; ++i) {
        int random_value = std::rand() % 2;
        if (random_value == 0) {
            auto& data = writer->open_data<Quote>(MTQuote);
            strcpy(data.symbol, "quote");
            data.price = 100;
            data.volume = 100;
            writer->close_data();
        } else {
            auto& data = writer->open_data<Order>(MTOrder);
            strcpy(data.symbol, "order");
            data.price = 2000;
            data.volume = 2000;
            writer->close_data();
        }
    }

    std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - start;
    std::cout << elapsed_seconds.count() << '\n';

    return 0;
}
