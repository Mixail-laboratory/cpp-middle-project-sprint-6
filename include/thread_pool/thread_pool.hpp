#pragma once
#include "queue/priority_queue.hpp"
#include <atomic>
#include <cstddef>
#include <memory>
#include <thread>
#include <vector>

namespace dispatcher::thread_pool {

class ThreadPool {
    std::vector<std::jthread> workers;
    std::shared_ptr<queue::PriorityQueue> queue_;
    void start();

public:
    explicit ThreadPool(size_t threads, const std::shared_ptr<queue::PriorityQueue> &queue);
    ~ThreadPool();
};

}  // namespace dispatcher::thread_pool
