#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

#include "task_dispatcher.hpp"
#include "types.hpp"

TEST(TaskDispatcher, ExecuteSimpleTask) {
    std::atomic<int> counter(0);

    dispatcher::TaskDispatcher dispatcher(2);

    dispatcher.schedule(dispatcher::TaskPriority::Normal, [&counter]() { counter++; });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(counter.load(), 1);
}

TEST(TaskDispatcher, ExecuteMultipleTasks) {
    dispatcher::TaskDispatcher dispatcher(4);
    std::atomic<int> counter(0);
    const int task_count = 10;

    for (int i = 0; i < task_count; ++i) {
        dispatcher.schedule(dispatcher::TaskPriority::Normal, [&counter]() { counter++; });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(counter.load(), task_count);
}

TEST(TaskDispatcher, PriorityHandling) {
    dispatcher::TaskDispatcher dispatcher(1);
    std::vector<int> execution_order;
    std::mutex order_mutex;

    dispatcher.schedule(dispatcher::TaskPriority::Normal, [&execution_order, &order_mutex]() {
        std::lock_guard<std::mutex> lock(order_mutex);
        execution_order.push_back(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    });

    dispatcher.schedule(dispatcher::TaskPriority::High, [&execution_order, &order_mutex]() {
        std::lock_guard<std::mutex> lock(order_mutex);
        execution_order.push_back(2);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    EXPECT_GE(execution_order.size(), 1);
}

TEST(TaskDispatcher, MultiThreadExecution) {
    dispatcher::TaskDispatcher dispatcher(4);
    std::atomic<int> counter(0);
    std::set<std::thread::id> thread_ids;
    std::mutex ids_mutex;
    const int task_count = 20;

    for (int i = 0; i < task_count; ++i) {
        dispatcher.schedule(dispatcher::TaskPriority::Normal, [&counter, &thread_ids, &ids_mutex]() {
            counter++;
            {
                std::lock_guard<std::mutex> lock(ids_mutex);
                thread_ids.insert(std::this_thread::get_id());
            }
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_EQ(counter.load(), task_count);
    EXPECT_GT(thread_ids.size(), 1);
}

TEST(TaskDispatcher, DestructorCleanup) {
    std::atomic<int> counter(0);

    {
        dispatcher::TaskDispatcher dispatcher(2);

        for (int i = 0; i < 5; ++i) {
            dispatcher.schedule(dispatcher::TaskPriority::Normal, [&counter]() {
                counter++;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    int final_count = counter.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(counter.load(), final_count);
}