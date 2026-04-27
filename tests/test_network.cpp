#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include "net/udp_receiver.hpp"
#include "net/tick_sender.hpp"

int main() {
    constexpr uint16_t PORT = 19000;
    std::atomic<int> received{0};

    std::thread receiver_thread([&]() {
        fasttrade::UDPReceiver receiver(PORT, [&](const fasttrade::MarketTick& tick) {
            ++received;
            std::cout << "Tick #" << received
                      << " | instrument=" << tick.instrument_id
                      << " | price=" << tick.price
                      << " | qty=" << tick.quantity << "\n";
        });
        while (received < 5) receiver.poll(50);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::thread sender_thread([&]() {
        fasttrade::TickSender sender("127.0.0.1", PORT);
        for (int i = 0; i < 5; ++i) {
            fasttrade::MarketTick tick{};
            tick.instrument_id = 1001 + i;
            tick.price         = 500.0 + i * 0.25;
            tick.quantity      = 100 * (i + 1);
            tick.timestamp_ns  = i;
            sender.send_tick(tick);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    sender_thread.join();
    receiver_thread.join();

    std::cout << "\nNetwork layer works. " << received << " ticks received.\n";
    return 0;
}
