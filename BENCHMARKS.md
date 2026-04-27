# FastTrade — Performance Benchmarks

> Machine: WSL2 Ubuntu 24.04 | 12-core x86_64 @ 2496 MHz | clang++-14 | -O3 -march=native

---

## Phase 1 — Core Data Structures

### SPSC Ring Buffer vs std::queue

| Metric               | SPSCRingBuffer  | std::queue + mutex |
|----------------------|-----------------|--------------------|
| Time per operation   | **1.41 ns**     | 9.27 ns            |
| CPU time per op      | 1.48 ns         | 9.54 ns            |
| Iterations run       | 446,511,612     | 77,035,817         |
| Throughput (ops/sec) | ~709 million    | ~107 million       |
| Speedup              | **6.5x faster** | baseline           |

### Why it's faster

| Optimization        | What it does                                       |
|---------------------|----------------------------------------------------|
| Lock-free design    | Atomic sequence numbers replace mutex locks        |
| Cache-aligned slots | `alignas(64)` puts producer/consumer on separate cache lines — prevents false sharing |
| Bitwise index wrap  | `head & (Capacity-1)` replaces `%` modulo — faster indexing |

> Note: benchmark library built in DEBUG mode — release numbers will be lower still.
