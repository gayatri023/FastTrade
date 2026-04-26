#include <iostream>
#include <thread>
#include "core/ring_buffer.hpp"

int main() {
    fasttrade::SPSCRingBuffer<int, 64> rb;

    std::thread producer([&]() {
        for (int i = 1; i <= 10; ++i) {
            while (!rb.push(i)) {}
            std::cout << "Pushed: " << i << "\n";
        }
    });

    std::thread consumer([&]() {
        int count = 0;
        while (count < 10) {
            auto val = rb.pop();
            if (val) {
                std::cout << "Got:    " << *val << "\n";
                ++count;
            }
        }
    });

    producer.join();
    consumer.join();
    std::cout << "\nRing buffer works.\n";
    return 0;
}
