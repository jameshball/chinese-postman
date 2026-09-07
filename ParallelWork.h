#pragma once
#include <atomic>
#include <future>
#include <vector>
#include <algorithm>
#include <system_error>
// Nested loops share one worker budget; the caller also consumes jobs.
class ParallelWork {
public:
    explicit ParallelWork(int workers) : workerCount(std::max(1, workers)), available(workerCount - 1) {}
    template<class Function> void forEach(size_t count, Function function, bool parallel = true) {
        if (!parallel || count < 2 || workerCount == 1) {
            for (size_t i = 0; i < count; ++i) { function(i); }
            return;
        }
        std::atomic<size_t> next{0};
        auto consume = [&] {
            for (;;) {
                const auto index = next.fetch_add(1, std::memory_order_relaxed);
                if (index >= count) { return; }
                function(index);
            }
        };
        const auto lanes = std::min(count, workerCount);
        std::vector<std::future<void>> tasks;
        tasks.reserve(lanes > 0 ? lanes - 1 : 0);
        for (size_t i = 1; i < lanes; ++i) {
            int slots = available.load();
            while (slots > 0 && !available.compare_exchange_weak(slots, slots - 1)) {}
            if (slots == 0) { break; }
            try {
                tasks.push_back(std::async(std::launch::async, [&] {
                    struct Release { std::atomic<int>& slots; ~Release() { ++slots; } } release{available};
                    consume();
                }));
            } catch (const std::system_error&) {
                ++available;
                break; // Complete the remaining work here if a thread cannot be started.
            } catch (...) {
                ++available;
                throw;
            }
        }
        consume();
        for (auto& task : tasks) { task.get(); }
    }
private:
    const size_t workerCount;
    std::atomic<int> available;
};
