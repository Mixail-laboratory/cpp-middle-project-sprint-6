#include "thread_pool/thread_pool.hpp"
#include "queue/priority_queue.hpp"
#include <atomic>
#include <cstddef>
#include <memory>
#include <print>
#include <thread>

namespace dispatcher::thread_pool {

ThreadPool::ThreadPool(size_t threads, const std::shared_ptr<queue::PriorityQueue> &queue) : queue_(queue) {
    workers.reserve(threads);
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back(&ThreadPool::start, this);
    }
}

void ThreadPool::start() {
    while (!stop.load(std::memory_order_acquire)) {
        const auto task = queue_->pop();
        if (task.has_value()) {
            try {
                task.value()();
                std::println("task executed!");
            } catch (const std::exception &ex) {
                std::print("exception in task: {}", ex.what());
            }
        } else {
            std::this_thread::yield();
            if (stop.load(std::memory_order_acquire)) {
                break;
            }
        }
    }
}

ThreadPool::~ThreadPool() {
    std::println("destroy pool");
    stop.store(true, std::memory_order_release);
    queue_->shutdown();
    for (auto &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    std::println("destroy pool done");
}

}  // namespace dispatcher::thread_pool