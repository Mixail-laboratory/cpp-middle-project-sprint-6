#include "queue/priority_queue.hpp"
#include "queue/bounded_queue.hpp"
#include "queue/queue.hpp"
#include "queue/unbounded_queue.hpp"
#include "types.hpp"
#include <atomic>
#include <memory>
#include <optional>
#include <print>
#include <semaphore>

namespace dispatcher::queue {

PriorityQueue::PriorityQueue(const std::map<TaskPriority, QueueOptions> &options) : semaphore_(0) {
    for (const auto &[priority, option] : options) {
        if (option.bounded) {
            queue_map_.try_emplace(priority, std::make_shared<BoundedQueue>(option.capacity.value()));
        } else {
            queue_map_.try_emplace(priority, std::make_shared<UnboundedQueue>());
        }
    }
}

void PriorityQueue::push(TaskPriority priority, std::function<void()> task) {
    pending_ops.fetch_add(1, std::memory_order_acq_rel);
    if (!is_active_.load(std::memory_order_acquire)) {
        pending_ops.fetch_sub(1, std::memory_order_acq_rel);
        return;
    }
    try {
        auto &queue = queue_map_.at(priority);
        queue->push(std::move(task));
        semaphore_.release();
    } catch (const std::exception &ex) {
        std::print("{}", ex.what());
    }
    pending_ops.fetch_sub(1, std::memory_order_acq_rel);
}

std::optional<std::function<void()>> PriorityQueue::pop() {
    for (const auto &[priority, queue] : queue_map_) {
        const auto task = queue->try_pop();
        if (task.has_value()) {
            return task;
        }
    }
    if (!is_active_.load(std::memory_order_acquire)) {
        return std::nullopt;
    }
    semaphore_.acquire();
    for (const auto &[priority, queue] : queue_map_) {
        const auto task = queue->try_pop();
        if (task.has_value()) {
            return task;
        }
    }
    return std::nullopt;
};

void PriorityQueue::shutdown() {
    is_active_.store(false, std::memory_order_relaxed);
    while (pending_ops.load(std::memory_order_acquire) > 0) {
        std::this_thread::yield();
    }
    for (int i = 0; i < semaphore_.max(); ++i) {
        semaphore_.release();
    }
}

}  // namespace dispatcher::queue