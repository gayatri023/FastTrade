#pragma once
#include <map>
#include <deque>
#include <vector>
#include <cstdint>
#include <iostream>
namespace fasttrade {
enum class Side { BUY, SELL };
struct Order { uint64_t id; Side side; double price; uint32_t quantity; };
struct Trade { uint64_t buy_order_id; uint64_t sell_order_id; double price; uint32_t quantity; };
class OrderBook {
public:
    void add_order(const Order& order) {
        if (order.side == Side::BUY) try_match_buy(order);
        else try_match_sell(order);
    }
    const std::vector<Trade>& trades() const { return trades_; }
    void print_book() const {
        std::cout << "\n=== ORDER BOOK ===\n";
        std::cout << "ASKS:\n";
        for (auto& [price, orders] : asks_)
            for (auto& o : orders)
                std::cout << "  " << price << " x " << o.quantity << "\n";
        std::cout << "BIDS:\n";
        for (auto it = bids_.rbegin(); it != bids_.rend(); ++it)
            for (auto& o : it->second)
                std::cout << "  " << it->first << " x " << o.quantity << "\n";
        std::cout << "==================\n";
    }
private:
    void try_match_buy(Order buy) {
        while (buy.quantity > 0 && !asks_.empty()) {
            auto it = asks_.begin();
            if (buy.price < it->first) break;
            auto& q = it->second;
            Order& sell = q.front();
            uint32_t matched = std::min(buy.quantity, sell.quantity);
            trades_.push_back({buy.id, sell.id, it->first, matched});
            std::cout << "TRADE: " << matched << " @ " << it->first << "\n";
            buy.quantity -= matched;
            sell.quantity -= matched;
            if (sell.quantity == 0) { q.pop_front(); if (q.empty()) asks_.erase(it); }
        }
        if (buy.quantity > 0) bids_[buy.price].push_back(buy);
    }
    void try_match_sell(Order sell) {
        while (sell.quantity > 0 && !bids_.empty()) {
            auto it = bids_.rbegin();
            if (sell.price > it->first) break;
            auto& q = it->second;
            Order& buy = q.front();
            uint32_t matched = std::min(sell.quantity, buy.quantity);
            trades_.push_back({buy.id, sell.id, it->first, matched});
            std::cout << "TRADE: " << matched << " @ " << it->first << "\n";
            sell.quantity -= matched;
            buy.quantity -= matched;
            if (buy.quantity == 0) { q.pop_front(); if (q.empty()) bids_.erase((++it).base()); }
        }
        if (sell.quantity > 0) asks_[sell.price].push_back(sell);
    }
    std::map<double, std::deque<Order>> bids_;
    std::map<double, std::deque<Order>> asks_;
    std::vector<Trade> trades_;
};
} // namespace fasttrade
