#include <benchmark/benchmark.h>
#include <queue>
#include <mutex>
#include "core/ring_buffer.hpp"

static void BM_SPSCRingBuffer(benchmark::State& state) {
    fasttrade::SPSCRingBuffer<int, 1024> rb;
    for (auto _ : state) {
        rb.push(42);
        auto val = rb.pop();
        benchmark::DoNotOptimize(val);
    }
}

static void BM_StdQueue(benchmark::State& state) {
    std::queue<int> q;
    std::mutex mtx;
    for (auto _ : state) {
        { std::lock_guard<std::mutex> lock(mtx); q.push(42); }
        { std::lock_guard<std::mutex> lock(mtx); q.pop(); }
    }
}

BENCHMARK(BM_SPSCRingBuffer);
BENCHMARK(BM_StdQueue);
BENCHMARK_MAIN();
