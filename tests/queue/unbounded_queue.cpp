#include <gtest/gtest.h>

#include "queue/unbounded_queue.hpp"

TEST(UnboundedQueue, PushAndPop) {
    dispatcher::queue::UnboundedQueue queue;
    queue.push([]() {});
    auto task = queue.try_pop();
    EXPECT_TRUE(task.has_value());
}

TEST(UnboundedQueue, NoCapacityLimit) {
    dispatcher::queue::UnboundedQueue queue;

    // ✅ Должна вместить любое количество
    for (int i = 0; i < 1000; ++i) {
        queue.push([]() {});
    }

    EXPECT_TRUE(queue.try_pop().has_value());
}

TEST(UnboundedQueue, EmptyQueueReturnsNullopt) {
    dispatcher::queue::UnboundedQueue queue;
    auto task = queue.try_pop();
    EXPECT_FALSE(task.has_value());
}

TEST(UnboundedQueue, MultiplePushPop) {
    dispatcher::queue::UnboundedQueue queue;
    int count = 0;

    for (int i = 0; i < 100; ++i) {
        queue.push([&count]() { count++; });
    }

    for (int i = 0; i < 100; ++i) {
        auto task = queue.try_pop();
        if (task.has_value())
            (*task)();
    }

    EXPECT_EQ(count, 100);
}

TEST(UnboundedQueue, ThreadSafety) {
    dispatcher::queue::UnboundedQueue queue;
    std::atomic<int> counter(0);

    std::thread producer([&queue, &counter]() {
        for (int i = 0; i < 100; ++i) {
            queue.push([&counter]() { counter++; });
        }
    });

    std::thread consumer([&queue]() {
        for (int i = 0; i < 100; ++i) {
            auto task = queue.try_pop();
            if (task.has_value())
                (*task)();
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(counter.load(), 100);
}