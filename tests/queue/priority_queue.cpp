#include <gtest/gtest.h>

#include "queue/priority_queue.hpp"

TEST(PriorityQueue, PushAndPopNormal) {
    std::map<dispatcher::TaskPriority, dispatcher::queue::QueueOptions> options = {
        {dispatcher::TaskPriority::Normal, {false, std::nullopt}},
        {dispatcher::TaskPriority::High, {false, std::nullopt}}};

    dispatcher::queue::PriorityQueue queue(options);
    queue.push(dispatcher::TaskPriority::Normal, []() {});
    auto task = queue.pop();
    EXPECT_TRUE(task.has_value());
}

TEST(PriorityQueue, HighPriorityFirst) {
    std::map<dispatcher::TaskPriority, dispatcher::queue::QueueOptions> options = {
        {dispatcher::TaskPriority::Normal, {false, std::nullopt}},
        {dispatcher::TaskPriority::High, {false, std::nullopt}}};

    dispatcher::queue::PriorityQueue queue(options);
    int order = 0;

    queue.push(dispatcher::TaskPriority::Normal, [&order]() { order = 1; });
    queue.push(dispatcher::TaskPriority::High, [&order]() { order = 2; });

    auto task1 = queue.pop();
    if (task1.has_value())
        (*task1)();

    EXPECT_EQ(order, 2);
}

TEST(PriorityQueue, MultiplePriorities) {
    std::map<dispatcher::TaskPriority, dispatcher::queue::QueueOptions> options = {
        {dispatcher::TaskPriority::Normal, {false, std::nullopt}},
        {dispatcher::TaskPriority::High, {false, std::nullopt}}};

    dispatcher::queue::PriorityQueue queue(options);
    int count = 0;

    queue.push(dispatcher::TaskPriority::Normal, [&count]() { count++; });
    queue.push(dispatcher::TaskPriority::High, [&count]() { count++; });
    queue.push(dispatcher::TaskPriority::Normal, [&count]() { count++; });

    for (int i = 0; i < 3; ++i) {
        auto task = queue.pop();
        if (task.has_value())
            (*task)();
    }

    EXPECT_EQ(count, 3);
}

TEST(PriorityQueue, ShutdownStopsPopping) {
    std::map<dispatcher::TaskPriority, dispatcher::queue::QueueOptions> options = {
        {dispatcher::TaskPriority::Normal, {false, std::nullopt}},
        {dispatcher::TaskPriority::High, {false, std::nullopt}}};

    dispatcher::queue::PriorityQueue queue(options);
    queue.push(dispatcher::TaskPriority::Normal, []() {});

    queue.shutdown();

    auto task = queue.pop();
    EXPECT_TRUE(task.has_value());
}

TEST(PriorityQueue, BoundedQueues) {
    std::map<dispatcher::TaskPriority, dispatcher::queue::QueueOptions> options = {
        {dispatcher::TaskPriority::Normal, {true, 5}}, {dispatcher::TaskPriority::High, {true, 3}}};

    dispatcher::queue::PriorityQueue queue(options);
    int count = 0;

    for (int i = 0; i < 5; ++i) {
        queue.push(dispatcher::TaskPriority::Normal, [&count]() { count++; });
    }

    for (int i = 0; i < 5; ++i) {
        auto task = queue.pop();
        if (task.has_value())
            (*task)();
    }

    EXPECT_EQ(count, 5);
}