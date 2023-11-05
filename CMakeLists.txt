cmake_minimum_required (VERSION 3.5)

project (concurrency-in-action-examples)

set (CMAKE_CXX_STANDARD 17)
enable_testing ()

add_library (catchMain OBJECT test/catch_main.cpp)

add_library (lockBasedDataStructures INTERFACE
    include/lock-based/threadsafe_stack.h
    include/lock-based/threadsafe_queue.h
)

add_executable (test-threadsafe-queue test/test-threadsafe-queue.cpp)
target_include_directories(test-threadsafe-queue PRIVATE include)
target_link_libraries (test-threadsafe-queue lockBasedDataStructures catchMain)
add_test(NAME TestThreadsafeQueue
    COMMAND test-threadsafe-queue
)
