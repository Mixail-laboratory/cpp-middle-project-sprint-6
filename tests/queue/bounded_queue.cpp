#include <gtest/gtest.h>

#include "queue/bounded_queue.hpp"

TEST(BoundedQueue, PushAndPop) {
    dispatcher::queue::BoundedQueue queue(5);
    queue.push([]() {});
    auto task = queue.try_pop();
    EXPECT_TRUE(task.has_value());
}

TEST(BoundedQueue, EmptyQueueReturnsNullopt) {
    dispatcher::queue::BoundedQueue queue(5);
    auto task = queue.try_pop();
    EXPECT_FALSE(task.has_value());
}

TEST(BoundedQueue, MultiplePushPop) {
    dispatcher::queue::BoundedQueue queue(10);
    int count = 0;

    for (int i = 0; i < 5; ++i) {
        queue.push([&count]() { count++; });
    }

    for (int i = 0; i < 5; ++i) {
        auto task = queue.try_pop();
        if (task.has_value())
            (*task)();
    }

    EXPECT_EQ(count, 5);
}

TEST(BoundedQueue, ThreadSafety) {
    dispatcher::queue::BoundedQueue queue(100);
    std::atomic<int> counter(0);

    std::thread producer([&queue, &counter]() {
        for (int i = 0; i < 50; ++i) {
            queue.push([&counter]() { counter++; });
        }
    });

    std::thread consumer([&queue]() {
        for (int i = 0; i < 50; ++i) {
            auto task = queue.try_pop();
            if (task.has_value())
                (*task)();
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(counter.load(), 50);
}