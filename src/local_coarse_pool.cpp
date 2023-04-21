/**
 * @file local_coarse_pool.cpp
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 18 2023
 *
 * This is an implementation file that implements the second version of
 * threadpool using thread-local work queue but coarse locking
 */

#include "local_coarse_pool.h"

#include <iostream>

LocalCoarsePool::LocalCoarsePool(int concurrency, PoolType pool_type)
    : BasePool(concurrency, pool_type) {
  for (int i = 0; i < concurrency_; i++) {
    // create padded resources
    auto r = std::make_unique<PaddedResource>();
    resources_.push_back(std::move(r));
    // create thread worker
    threads_.emplace_back([this, id = i] {
      // in BATCH mode, wait for signal
      while (status_ == PoolStatus::PREPARE) {
      };
      // enter main loop of polling and execution
      while (true) {
        Task next_task;
        {
          // wait for either a task available, or exit signal
          std::unique_lock<std::mutex> lock(resources_[id]->mtx);
          resources_[id]->cv.wait(lock, [this, id]() -> bool {
            return status_ == PoolStatus::EXIT ||
                   !resources_[id]->queue.empty();
          });
          if (resources_[id]->queue.empty() && status_ == PoolStatus::EXIT) {
            // this pool is about to be destroyed
            return;
          }
          next_task = resources_[id]->queue.front();
          resources_[id]->queue.pop();
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

LocalCoarsePool::~LocalCoarsePool() {
  // force signal and clear
  Exit();
  // harvest all worker threads
  for (auto& worker : threads_) {
      worker.join();
  }
}

void LocalCoarsePool::Submit(Task task) {
  assert(status_ != PoolStatus::EXIT);
  // Round-robin load balancer
  int robin = submit_count_.fetch_add(1);  // atmoic add
  int i = robin % concurrency_;
  {
    // does this create contention? but seems unavoidable
    std::unique_lock<std::mutex> lock(resources_[i]->mtx);
    resources_[i]->queue.push(std::move(task));
  }
  resources_[i]->cv.notify_all();
}

void LocalCoarsePool::WaitUntilFinished() {
  std::unique_lock<std::mutex> lock(mtx_count_);
  cv_count_.wait(lock,
                 [this]() -> bool { return submit_count_ == finish_count_; });
}

void LocalCoarsePool::Exit() {
  status_ = PoolStatus::EXIT;
  for (int i = 0; i < concurrency_; i++) {
    // wake up sleeping worker
    resources_[i]->cv.notify_all();
  }
}