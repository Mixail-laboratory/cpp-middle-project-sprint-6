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
    while (true) {
        const auto task = queue_->pop();
        if (task.has_value()) {
            try {
                task.value()();
            } catch (const std::exception &ex) {
                std::print("exception in task: {}", ex.what());
            }
        } else {
            break;
        }
        std::this_thread::yield();
    }
}

ThreadPool::~ThreadPool() {
    queue_->shutdown();
    for (auto &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

}  // namespace dispatcher::thread_pool