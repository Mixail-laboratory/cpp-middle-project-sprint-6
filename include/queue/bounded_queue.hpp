#pragma once
#include "queue/queue.hpp"
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <mutex>
namespace dispatcher::queue {

class BoundedQueue : public IQueue {
    std::deque<std::function<void()>> queue_;
    size_t capacity_;
    std::mutex mutex_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;

public:
    explicit BoundedQueue(size_t capacity);

    void push(std::function<void()> task) override;

    std::optional<std::function<void()>> try_pop() override;

    ~BoundedQueue() override = default;
};

}  // namespace dispatcher::queue