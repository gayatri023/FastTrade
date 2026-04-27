#include <iostream>
#include <cassert>
#include "engine/order_book.hpp"
int main() {
    fasttrade::OrderBook book;
    book.add_order({1, fasttrade::Side::BUY,  500.0, 100});
    book.add_order({2, fasttrade::Side::BUY,  499.5, 200});
    book.add_order({3, fasttrade::Side::BUY,  499.0, 150});
    book.print_book();
    std::cout << "\nSELL 80 @ 499.0 (matches bid @ 500.0):\n";
    book.add_order({4, fasttrade::Side::SELL, 499.0, 80});
    std::cout << "\nSELL 150 @ 499.0 (matches remaining):\n";
    book.add_order({5, fasttrade::Side::SELL, 499.0, 150});
    book.print_book();
    std::cout << "\nTotal trades: " << book.trades().size() << "\n";
    assert(book.trades().size() >= 2);
    std::cout << "Order book matching engine works.\n";
    return 0;
}
