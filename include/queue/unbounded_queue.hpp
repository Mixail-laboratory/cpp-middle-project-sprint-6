#pragma once
#include "queue/queue.hpp"
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>

namespace dispatcher::queue {

class UnboundedQueue : public IQueue {
    std::deque<std::function<void()>> queue_;
    std::mutex mutex_;
    std::condition_variable not_empty_;

public:
    explicit UnboundedQueue() = default;

    void push(std::function<void()> task) override;

    std::optional<std::function<void()>> try_pop() override;

    ~UnboundedQueue() override = default;
};

}  // namespace dispatcher::queue