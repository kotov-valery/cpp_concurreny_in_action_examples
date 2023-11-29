#pragma once

#include <exception>
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

/**
 * Thread safe queue based on linked list
 * with more fine grained locking
 */
template <typename T>
class threadsafe_queue2
{
public:
    threadsafe_queue2() : _head(new node), _tail(_head.get()) {}

    threadsafe_queue2(const threadsafe_queue2 &) = delete;
    threadsafe_queue2 &operator=(const threadsafe_queue2 &) = delete;

    std::shared_ptr<T> try_pop()
    {
        auto old_head = pop_head();
        return old_head ? old_head->data : std::shared_ptr<T>();
    }

    void push(T new_value)
    {
        auto new_data = std::make_shared<T>(std::move(new_value));
        auto new_node = std::make_unique<node>();
        const auto new_tail = new_node.get();
        std::lock_guard tail_lock{_tail_mutex};
        _tail->data = new_data;
        _tail->next = std::move(new_node);
        _tail = new_tail;
    }

private:
    struct node
    {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };
    std::mutex _head_mutex;
    std::unique_ptr<node> _head;
    std::mutex _tail_mutex;
    node *_tail;

    node *get_tail()
    {
        std::lock_guard tail_lock{_tail_mutex};
        return _tail;
    }

    std::unique_ptr<node> pop_head()
    {
        std::lock_guard head_lock{_head_mutex};
        if (_head.get() == get_tail())
        {
            return {};
        }
        auto old_head = std::move(_head);
        _head = std::move(old_head->next);
        return old_head;
    }
};
