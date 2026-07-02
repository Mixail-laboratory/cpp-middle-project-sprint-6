#pragma once
#include "queue/bounded_queue.hpp"
#include "queue/queue.hpp"
#include "queue/unbounded_queue.hpp"
#include "types.hpp"

#include <atomic>
#include <cstdint>
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
#include <sys/types.h>
#include <thread>
#include <unordered_map>

namespace dispatcher::queue {

class PriorityQueue {
    static const auto max_threads = 256;

    std::counting_semaphore<max_threads> semaphore_;
    std::atomic<bool> is_active_ = true;
    std::map<TaskPriority, std::shared_ptr<IQueue>> queue_map_;
    std::atomic<uint64_t> pending_ops{0};

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