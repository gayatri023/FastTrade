#pragma once
#include <atomic>
#include <cstdint>
#include <optional>

namespace fasttrade {

template<typename T, uint32_t Capacity>
class SPSCRingBuffer {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");

    struct Slot {
        T item;
        std::atomic<uint64_t> sequence{0};
    };

public:
    SPSCRingBuffer() {
        for (uint32_t i = 0; i < Capacity; ++i)
            slots_[i].sequence.store(i, std::memory_order_relaxed);
        head_.store(0, std::memory_order_relaxed);
        tail_.store(0, std::memory_order_relaxed);
    }

    bool push(const T& item) noexcept {
        uint64_t head = head_.load(std::memory_order_relaxed);
        Slot& slot = slots_[head & (Capacity - 1)];
        uint64_t seq = slot.sequence.load(std::memory_order_acquire);
        int64_t diff = static_cast<int64_t>(seq) - static_cast<int64_t>(head);
        if (diff < 0) return false;
        slot.item = item;
        slot.sequence.store(head + 1, std::memory_order_release);
        head_.store(head + 1, std::memory_order_relaxed);
        return true;
    }

    std::optional<T> pop() noexcept {
        uint64_t tail = tail_.load(std::memory_order_relaxed);
        Slot& slot = slots_[tail & (Capacity - 1)];
        uint64_t seq = slot.sequence.load(std::memory_order_acquire);
        int64_t diff = static_cast<int64_t>(seq) - static_cast<int64_t>(tail + 1);
        if (diff < 0) return std::nullopt;
        T item = slot.item;
        slot.sequence.store(tail + Capacity, std::memory_order_release);
        tail_.store(tail + 1, std::memory_order_relaxed);
        return item;
    }

    bool empty() const noexcept {
        return head_.load(std::memory_order_relaxed) == tail_.load(std::memory_order_relaxed);
    }

private:
    alignas(64) Slot slots_[Capacity];
    alignas(64) std::atomic<uint64_t> head_{0};
    alignas(64) std::atomic<uint64_t> tail_{0};
};

} // namespace fasttrade
