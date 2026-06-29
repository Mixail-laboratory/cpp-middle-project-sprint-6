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
#include <unordered_map>

namespace dispatcher::queue {

PriorityQueue::PriorityQueue(const std::unordered_map<TaskPriority, QueueOptions> &options) : semaphore_(0) {
    for (const auto &[priority, option] : options) {
        if (option.bounded) {
            queue_map_.try_emplace(priority, std::make_shared<BoundedQueue>(option.capacity.value()));
        } else {
            queue_map_.try_emplace(priority, std::make_shared<UnboundedQueue>());
        }
    }
}

void PriorityQueue::push(TaskPriority priority, std::function<void()> task) {
    std::println("push");
    if (!is_active_.load(std::memory_order_acquire)) {
        return;
    }
    try {
        auto &queue = queue_map_.at(priority);
        queue->push(std::move(task));
        semaphore_.release();
    } catch (const std::exception &ex) {
        std::print("{}", ex.what());
    }
}

std::optional<std::function<void()>> PriorityQueue::pop() {
    if (!is_active_.load(std::memory_order_acquire)) {
        return std::nullopt;
    }
    semaphore_.acquire();
    if (!is_active_.load(std::memory_order_acquire)) {
        return std::nullopt;
    }
    std::println("pop");
    for (const auto &priority : {TaskPriority::High, TaskPriority::Normal}) {
        if (queue_map_.contains(priority)) {
            const auto task = queue_map_.at(priority)->try_pop();
            if (task.has_value()) {
                return task;
            }
        }
    }
    std::println("pop: no task found");

    return std::nullopt;
};

void PriorityQueue::shutdown() {
    std::println("shutdown: start");
    is_active_.store(false, std::memory_order_relaxed);
    for (int i = 0; i < 100; ++i) {
        semaphore_.release();
    }
    std::println("shutdown stop");
}

}  // namespace dispatcher::queue