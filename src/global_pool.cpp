/**
 * @file global_pool.cpp
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 10 2023
 *
 * This is an implementation file that implements the first version of
 * threadpool using a global work queue and each worker polling from this single
 * queue for more task
 */

#include "global_pool.h"

#include <iostream>
GlobalPool::GlobalPool(int concurrency, PoolType pool_type)
    : BasePool(concurrency, pool_type) {
  // create thread worker
  for (auto i = 0; i < GetConcurrency(); i++) {
    threads_.emplace_back([this]() {
      // in BATCH mode, wait for signal
      while (status_ == PoolStatus::PREPARE) {
      };
      // enter main loop of polling and execution
      while (true) {
        Task next_task;
        {
          // wait for either a task available, or exit signal
          std::unique_lock<std::mutex> lock(mtx_);
          cv_.wait(lock, [this]() -> bool {
            return status_ == PoolStatus::EXIT || !task_queue_.empty();
          });
          if (task_queue_.empty() && status_ == PoolStatus::EXIT) {
            // this pool is about to be destroyed
            return;
          }
          next_task = task_queue_.front();
          task_queue_.pop();
        }
        next_task();
        // there is no add_fetch available
        int post_increment = finish_count_.fetch_add(1) + 1;
        if (post_increment == submit_count_) {
          // notify the WaitUntilFinished() caller
          cv_count_.notify_all();
        }
      }
    });
  }
}

GlobalPool::~GlobalPool() {
  {
    std::unique_lock<std::mutex> lock(mtx_);
    std::queue<Task> empty_queue;
    task_queue_.swap(empty_queue);
  }
  // harvest all worker threads
  for (auto& worker : threads_) {
    worker.join();
  }
}

void GlobalPool::Submit(Task task) {
  assert(status_ != PoolStatus::EXIT);
  {
    std::unique_lock<std::mutex> lock(mtx_);
    task_queue_.push(std::move(task));
  }
  submit_count_.fetch_add(1);  // atomic add
  cv_.notify_one();
}

void GlobalPool::WaitUntilFinished() {
  std::unique_lock<std::mutex> lock(mtx_count_);
  cv_count_.wait(lock,
                 [this]() -> bool { return submit_count_ == finish_count_; });
  finish_count_.store(0);
  submit_count_.store(0);
  printf("task count: %d\n", finish_count_.load());
  fflush(stdout);
}

void GlobalPool::Exit() {
  status_ = PoolStatus::EXIT;
  cv_.notify_all();  // wake up sleeping worker
}