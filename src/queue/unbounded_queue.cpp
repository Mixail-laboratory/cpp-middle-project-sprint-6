#include "queue/unbounded_queue.hpp"

#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <semaphore>

namespace dispatcher::queue {

void UnboundedQueue::push(std::function<void()> task) {
    std::unique_lock lock(mutex_);
    queue_.push_front(std::move(task));
}

std::optional<std::function<void()>> UnboundedQueue::try_pop() {
    std::unique_lock lock(mutex_);

    if (queue_.empty()) {
        return std::nullopt;
    }
    const auto task = queue_.front();
    queue_.pop_front();
    return task;
}

}  // namespace dispatcher::queue