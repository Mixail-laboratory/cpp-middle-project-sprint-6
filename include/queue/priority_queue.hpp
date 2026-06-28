#pragma once
#include "queue/bounded_queue.hpp"
#include "queue/queue.hpp"
#include "queue/unbounded_queue.hpp"
#include "types.hpp"

#include <atomic>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <semaphore>
#include <stdexcept>
#include <unordered_map>

namespace dispatcher::queue {

class PriorityQueue {
    std::binary_semaphore semaphore_;
    std::priority_queue<std::function<void()>> queue_;
    std::atomic<bool> is_active_ = true;
    std::unordered_map<TaskPriority, std::shared_ptr<IQueue>> queue_map_;

public:
    explicit PriorityQueue(const std::unordered_map<TaskPriority, QueueOptions> &options);

    void push(TaskPriority priority, std::function<void()> task);
    // block on pop until shutdown is called
    // after that return std::nullopt on empty queue
    std::optional<std::function<void()>> pop();

    void shutdown();

    ~PriorityQueue() = default;
};

}  // namespace dispatcher::queue