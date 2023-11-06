#include "catch2/catch.hpp"

#include <chrono>
#include <memory>
#include <thread>

#include <lock-based/threadsafe_queue.h>

using namespace std::chrono_literals;

TEST_CASE("Try pop on an empty queue") {
    threadsafe_queue<int> queue;
    REQUIRE(queue.empty());
    REQUIRE_FALSE(queue.try_pop());
}

TEST_CASE("Destroy during waiting") {
    threadsafe_queue<int> queue;
    auto destruction_thread = std::thread([&]{
        std::this_thread::sleep_for(200ms);
        queue.close();
    });
    const auto value = queue.wait_and_pop();
    REQUIRE_FALSE(value);
    destruction_thread.join();
}

TEST_CASE("Push and try pop") {
    threadsafe_queue<int> queue;
    const auto test_value = 5;
    queue.push(test_value);
    const auto value = queue.try_pop();
    REQUIRE(value);
    REQUIRE(test_value == *value);
}

TEST_CASE("Push and try pop by value") {
    threadsafe_queue<int> queue;
    const auto test_value = 5;
    queue.push(test_value);
    int value{};
    REQUIRE(queue.try_pop(value));
    REQUIRE(test_value == value);
}

TEST_CASE("Wait and pop and push") {
    const auto test_value = 5;

    threadsafe_queue<int> queue;
    auto push_thread = std::thread([&]{
        std::this_thread::sleep_for(200ms);
        queue.push(test_value);
    });

    const auto value = queue.wait_and_pop();
    REQUIRE(value);
    REQUIRE(test_value == *value);
    push_thread.join();
}

TEST_CASE("Wait and pop and push by value") {
    const auto test_value = 5;

    threadsafe_queue<int> queue;
    auto push_thread = std::thread([&]{
        std::this_thread::sleep_for(200ms);
        queue.push(test_value);
    });

    int value{}; ;
    queue.wait_and_pop(value);
    REQUIRE(test_value == value);
    push_thread.join();
}
