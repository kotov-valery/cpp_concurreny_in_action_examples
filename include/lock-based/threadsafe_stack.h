#pragma once

#include <exception>
#include <mutex>
#include <stack>

struct empty_stack: std::exception {
    const char* what() const throw();
};

template <typename T>
class threadsafe_stack
{
public:
    threadsafe_stack(){};
    threadsafe_stack(const threadsafe_stack& other) {
        std::lock_guard lock{other._mutex};
        _data = other._data;
    }

    threadsafe_stack& operator=(const threadsafe_stack& other) = delete;

    void push(T value) {
        std::lock_guard lock{_mutex};
        _data.push(std::move(value));
    }

    std::shared_ptr<T> pop() {
        std::lock_guard lock{_mutex};
        if (_data.empty()) throw empty_stack();
        const auto value{std::make_shared<T>(std::move(_data.top()))};
        _data.pop();
        return value;
    }

    void pop(T& value) {
        std::lock_guard lock{_mutex};
        if (_data.empty()) throw empty_stack();
        value = std::move(_data.top());
        _data.pop();
    }

    bool empty() const {
        std::lock_guard lock{_mutex};
        return _data.empty();
    }

private:
    std::stack<T> _data;
    mutable std::mutex _mutex;
};