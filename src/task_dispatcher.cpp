#include "task_dispatcher.hpp"
#include "queue/priority_queue.hpp"
#include "queue/queue.hpp"
#include "thread_pool/thread_pool.hpp"
#include "types.hpp"
#include <cstddef>
#include <memory>
#include <unordered_map>
#include <utility>

namespace dispatcher {

std::unordered_map<TaskPriority, queue::QueueOptions> TaskDispatcher::default_config() {
    std::unordered_map<TaskPriority, queue::QueueOptions> config;
    config[TaskPriority::High] = queue::QueueOptions(true, 1000);
    config[TaskPriority::Normal] = queue::QueueOptions(false);
    return config;
}

TaskDispatcher::TaskDispatcher(size_t thread_count, const std::unordered_map<TaskPriority, queue::QueueOptions> &option)
    : queue_(std::make_shared<queue::PriorityQueue>(option)),
      thread_pool_(std::make_unique<thread_pool::ThreadPool>(thread_count, queue_)) {}

void TaskDispatcher::schedule(TaskPriority priority, std::function<void()> task) { queue_->push(priority, task); }

}  // namespace dispatcher