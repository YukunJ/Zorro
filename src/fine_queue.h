/**
 * @file local_fine_pool.h
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 18 2023
 *
 * This is a header file that specifies the second version of threadpool
 * which is thread-local work queue with fine locking
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "base_pool.h"

template <typename T>
class fine_queue {
private:
    struct node {
        T data;
        node *next;
    };
    std::mutex head_mutex;
    node *head;
    std::mutex tail_mutex;
    node *tail;

    node *pop_head() {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head == tail) {
            return nullptr;
        }
        node *old_head = head;
        head = old_head->next;
        return old_head;
    }

public:
    fine_queue() : head(new node), tail(head) {}
    fine_queue(const fine_queue &other) = delete;
    fine_queue &operator=(const fine_queue &other) = delete;
    bool pop(T &task) {
        node *old_head = pop_head();
        if (old_head != nullptr) {
            task = std::move(old_head->data);
            delete old_head;
            return true;
        }
        return false;
    }
    void push(T new_value) {
        node *new_tail = new node;
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        tail->data = std::move(new_value);
        tail->next = new_tail;
        tail = new_tail;
    }
};

/* padded this struct to be at least multiples of cache-line width to avoid
 * false-sharing */
typedef struct PaddedResourceFine {
    fine_queue<Task> queue;
    std::mutex pop_mtx;
    std::mutex push_mtx;
    std::condition_variable cv;
} PaddedResourceFine __attribute__((aligned(256)));
