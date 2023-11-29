#include "catch2/catch.hpp"

#include <chrono>
#include <memory>
#include <thread>

#include <lock-based/threadsafe_queue2.h>

using namespace std::chrono_literals;

TEST_CASE("Q2: Push and try pop")
{
    threadsafe_queue2<int> queue;
    const auto test_value = 5;
    queue.push(test_value);
    const auto value = queue.try_pop();
    REQUIRE(value);
    REQUIRE(test_value == *value);
}
