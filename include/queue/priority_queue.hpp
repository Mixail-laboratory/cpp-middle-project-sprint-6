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
#include <print>
#include <queue>
#include <semaphore>
#include <stdexcept>
#include <thread>
#include <unordered_map>

namespace dispatcher::queue {

class PriorityQueue {
    std::counting_semaphore<256> semaphore_;
    std::atomic<bool> is_active_ = true;
    std::map<TaskPriority, std::shared_ptr<IQueue>> queue_map_;

public:
    explicit PriorityQueue(const std::map<TaskPriority, QueueOptions> &options);

    void push(TaskPriority priority, std::function<void()> task);
    // block on pop until shutdown is called
    // after that return std::nullopt on empty queue
    std::optional<std::function<void()>> pop();

    void shutdown();

    ~PriorityQueue() = default;
};

}  // namespace dispatcher::queue