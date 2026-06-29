#include "queue/bounded_queue.hpp"
#include <cstddef>
#include <functional>
#include <mutex>
#include <optional>

namespace dispatcher::queue {

BoundedQueue::BoundedQueue(size_t capacity) : capacity_(capacity) {}

void BoundedQueue::push(std::function<void()> func) {
    std::unique_lock lock(mutex_);
    not_full_.wait(lock, [this] { return queue_.size() < capacity_; });

    queue_.push_back(std::move(func));
}

std::optional<std::function<void()>> BoundedQueue::try_pop() {
    std::unique_lock lock(mutex_);

    if (queue_.empty()) {
        return std::nullopt;
    }
    const auto task = std::move(queue_.front());
    queue_.pop_front();
    not_full_.notify_one();
    return task;
}

}  // namespace dispatcher::queue