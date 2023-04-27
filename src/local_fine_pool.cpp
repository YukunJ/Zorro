/**
 * @file local_fine_pool.cpp
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 18 2023
 *
 * This is an implementation file that implements the second version of
 * threadpool using thread-local work queue but fine locking
 */

#include "local_fine_pool.h"

#include <iostream>

LocalFinePool::LocalFinePool(int concurrency, PoolType pool_type)
    : BasePool(concurrency, pool_type) {
  for (int i = 0; i < concurrency_; i++) {
    // create padded resources
    auto r = std::make_unique<PaddedResourceFine>();
    resources_.push_back(std::move(r));
    // create thread worker
    threads_.emplace_back([this, id = i] {
      // in BATCH mode, wait for signal
      while (status_ == PoolStatus::PREPARE) {
      };
      // enter main loop of polling and execution
      while (true) {
        Task next_task;
        bool has_next_task = false;
        {
          // wait for either a task available, or exit signal
          do {
            {
              //std::unique_lock<std::mutex> lock(resources_[id]->pop_mtx);
              has_next_task = resources_[id]->queue.pop(next_task);
            }
            if (!has_next_task) {
              std::this_thread::yield();
            }
          } while (!has_next_task && status_ != PoolStatus::EXIT);

          //          std::unique_lock<std::mutex>
          //          lock(resources_[id]->pop_mtx);
          //          resources_[id]->cv.wait(lock, [this, id = id, &next_task,
          //          &has_next_task]() -> bool {
          //            has_next_task = resources_[id]->queue.pop(next_task);
          //            return status_ == PoolStatus::EXIT || has_next_task;
          //          });
          if (!has_next_task && status_ == PoolStatus::EXIT) {
            // this pool is about to be destroyed
            return;
          }
        }
        next_task();
        // there is no add_fetch available
        int post_increment = finish_count_.fetch_add(1) + 1;
        // printf("Finished task %d\n", post_increment);
        // fflush(stdout);
        if (post_increment == submit_count_.load()) {
          // notify the WaitUntilFinished() caller
          cv_count_.notify_all();
        }
      }
    });
  }
}

LocalFinePool::~LocalFinePool() {
  // force signal and clear
  Exit();
  // harvest all worker threads
  for (auto& worker : threads_) {
    worker.join();
  }
}

void LocalFinePool::Submit(Task task) {
  assert(status_ != PoolStatus::EXIT);
  // Round-robin load balancer
  int robin = submit_count_.fetch_add(1);  // atmoic add
  int i = robin % concurrency_;
  {
    // does this create contention? but seems unavoidable
    // std::unique_lock<std::mutex> lock(resources_[i]->push_mtx);
    resources_[i]->queue.push(std::move(task));
    // printf("Pushed task %d\n", robin);
    // fflush(stdout);
  }
  resources_[i]->cv.notify_all();
}

void LocalFinePool::WaitUntilFinished() {
  std::unique_lock<std::mutex> lock(mtx_count_);
    std::cout << "without stealing" << std::endl;
    fflush(stdout);
  if (submit_count_.load() == finish_count_.load()) {
      Exit();
      return;
  }
  cv_count_.wait(lock,
                 [this]() -> bool { return submit_count_.load() == finish_count_.load(); });
  Exit();
}

void LocalFinePool::Exit() {
  status_ = PoolStatus::EXIT;
  for (int i = 0; i < concurrency_; i++) {
    // wake up sleeping worker
    resources_[i]->cv.notify_all();
  }
}