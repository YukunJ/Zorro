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
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };
    // std::mutex head_mutex;
    std::unique_ptr<node> head;
    // std::mutex tail_mutex;
    node *tail;

    std::unique_ptr<node> pop_head() {
        // std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head.get() == tail) {
            return nullptr;
        }
        std::unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }
public:
    fine_queue():head(new node), tail(head.get()) {}
    fine_queue(const fine_queue& other) = delete;
    fine_queue& operator=(const fine_queue& other) = delete;
    std::shared_ptr<T> pop() {
        std::unique_ptr<node> old_head = pop_head();
        return old_head?old_head->data:nullptr;
    }
    void push(T new_value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
        std::unique_ptr<node> p(new node);
        node * const new_tail = p.get();
        // std::lock_guard<std::mutex> tail_lock(tail_mutex);
        tail->data = new_data;
        tail->next = std::move(p);
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

class LocalFinePool final : public BasePool {
 public:
  LocalFinePool(int concurrency, PoolType pool_type);

  ~LocalFinePool();

  void Submit(Task task) override;

  void WaitUntilFinished() override;

  void Exit();

 private:
  std::atomic<int> submit_count_{0};
  std::atomic<int> finish_count_{0};
  std::vector<std::thread> threads_;
  std::vector<std::unique_ptr<PaddedResourceFine>> resources_;
  std::mutex mtx_count_;
  std::condition_variable cv_count_;
};