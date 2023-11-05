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
    threadsafe_queue<int> queue_ptr;
    auto destruction_thread = std::thread([&]{
        std::this_thread::sleep_for(200ms);
        queue_ptr.close();
    });
    const auto value = queue_ptr.wait_and_pop();
    REQUIRE_FALSE(value);
    destruction_thread.join();
}
