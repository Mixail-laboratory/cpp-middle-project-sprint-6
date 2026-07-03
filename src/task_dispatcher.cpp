#include "task_dispatcher.hpp"
#include "queue/priority_queue.hpp"
#include "queue/queue.hpp"
#include "thread_pool/thread_pool.hpp"
#include "types.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>

namespace dispatcher {

std::map<TaskPriority, queue::QueueOptions> TaskDispatcher::default_config() {
    std::map<TaskPriority, queue::QueueOptions> config;
    config[TaskPriority::High] = queue::QueueOptions{.bounded = true, .capacity = 1000};
    config[TaskPriority::Normal] = queue::QueueOptions{.bounded = false, .capacity = std::nullopt};
    return config;
}

TaskDispatcher::TaskDispatcher(size_t thread_count, const std::map<TaskPriority, queue::QueueOptions> &option)
    : queue_(std::make_shared<queue::PriorityQueue>(option)),
      thread_pool_(std::make_unique<thread_pool::ThreadPool>(thread_count, queue_)) {}

void TaskDispatcher::schedule(TaskPriority priority, std::function<void()> task) {
    queue_->push(priority, std::move(task));
}

}  // namespace dispatcher