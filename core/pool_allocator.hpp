#pragma once
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <new>

namespace fasttrade {

template<typename T, size_t PoolSize>
class PoolAllocator {
    union Slot {
        T object;
        Slot* next_free;
        Slot() {}
        ~Slot() {}
    };

public:
    PoolAllocator() {
        pool_ = static_cast<Slot*>(std::aligned_alloc(alignof(Slot), sizeof(Slot) * PoolSize));
        assert(pool_ != nullptr);
        for (size_t i = 0; i < PoolSize - 1; ++i)
            pool_[i].next_free = &pool_[i + 1];
        pool_[PoolSize - 1].next_free = nullptr;
        free_head_ = &pool_[0];
        allocated_count_ = 0;
        total_allocations_ = 0;
    }

    ~PoolAllocator() { std::free(pool_); }
    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator& operator=(const PoolAllocator&) = delete;

    template<typename... Args>
    T* allocate(Args&&... args) noexcept {
        if (free_head_ == nullptr) return nullptr;
        Slot* slot = free_head_;
        free_head_ = slot->next_free;
        T* obj = new (&slot->object) T(std::forward<Args>(args)...);
        ++allocated_count_;
        ++total_allocations_;
        return obj;
    }

    void free(T* obj) noexcept {
        if (obj == nullptr) return;
        obj->~T();
        Slot* slot = reinterpret_cast<Slot*>(obj);
        slot->next_free = free_head_;
        free_head_ = slot;
        --allocated_count_;
    }

    size_t slots_in_use()    const noexcept { return allocated_count_; }
    size_t slots_available() const noexcept { return PoolSize - allocated_count_; }
    size_t total_allocated() const noexcept { return total_allocations_; }
    bool   is_full()         const noexcept { return free_head_ == nullptr; }

private:
    Slot*  pool_;
    Slot*  free_head_;
    size_t allocated_count_;
    size_t total_allocations_;
};

} // namespace fasttrade
