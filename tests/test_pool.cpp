#include <iostream>
#include <cassert>
#include "core/pool_allocator.hpp"

struct Order {
    uint64_t id;
    double   price;
    int32_t  quantity;
};

int main() {
    fasttrade::PoolAllocator<Order, 1024> pool;
    std::cout << "Slots available: " << pool.slots_available() << "\n";

    Order* orders[10];
    for (int i = 0; i < 10; ++i) {
        orders[i] = pool.allocate();
        orders[i]->id = i;
        orders[i]->price = 100.0 + i;
        orders[i]->quantity = 50;
    }
    std::cout << "After 10 allocs. In use: " << pool.slots_in_use() << "\n";

    for (int i = 0; i < 5; ++i) pool.free(orders[i]);
    std::cout << "After 5 frees. In use: " << pool.slots_in_use() << "\n";

    Order* reused = pool.allocate();
    reused->id = 999;
    std::cout << "Reused slot ID: " << reused->id << "\n";
    std::cout << "Total ever allocated: " << pool.total_allocated() << "\n";
    assert(pool.slots_in_use() == 6);
    std::cout << "\nPool allocator works.\n";
    return 0;
}
