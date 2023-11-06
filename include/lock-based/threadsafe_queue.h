#pragma once

#include <exception>
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

template <typename T>
class threadsafe_queue
{
public:
    threadsafe_queue() {}

    void close() {
        std::lock_guard lock{_mutex};
        _close = true;
        _data_cond.notify_all();
    }

    void push(T new_value)
    {
        std::shared_ptr<T> data{
            std::make_shared<T>(std::move(new_value))
        };
        std::lock_guard lock{_mutex};
        _data_queue.push(data);
        _data_cond.notify_one();
    }

    void wait_and_pop(T &value)
    {
        std::unique_lock lock{_mutex};
        _data_cond.wait(lock, [this]
                        { return _close || !_data_queue.empty(); });
        if (_close) { return; }
        value = std::move(*_data_queue.front());
        _data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock lock{_mutex};
        _data_cond.wait(lock, [this]
                        { return _close || !_data_queue.empty(); });
        if (_close) { return {}; }
        auto value = _data_queue.front();
        _data_queue.pop();
        return value;
    }

    bool try_pop(T &value)
    {
        std::lock_guard lock{_mutex};
        if (_data_queue.empty()) {
            return false;
        }
        value = std::move(*_data_queue.front());
        _data_queue.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard lock{_mutex};
        if (_data_queue.empty()) {
            return {};
        }
        auto value = _data_queue.front();
        _data_queue.pop();
        return value;
    }

    bool empty() const
    {
        std::lock_guard lock{_mutex};
        return _data_queue.empty();
    }

private:
    mutable std::mutex _mutex;
    std::queue<std::shared_ptr<T>> _data_queue;
    std::condition_variable _data_cond;
    bool _close{false};
};
