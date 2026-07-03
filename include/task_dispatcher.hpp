#pragma once

#include <memory>
#include <print>
#include <unordered_map>

#include "queue/priority_queue.hpp"
#include "queue/queue.hpp"
#include "thread_pool/thread_pool.hpp"
#include "types.hpp"

namespace dispatcher {

class TaskDispatcher {
    std::shared_ptr<queue::PriorityQueue> queue_;
    std::unique_ptr<thread_pool::ThreadPool> thread_pool_;

public:
    TaskDispatcher(size_t thread_count, const std::map<TaskPriority, queue::QueueOptions> &option = default_config());

    void schedule(TaskPriority priority, std::function<void()> task);
    ~TaskDispatcher() { queue_->shutdown(); };

private:
    static std::map<TaskPriority, queue::QueueOptions> default_config();
};

}  // namespace dispatcher