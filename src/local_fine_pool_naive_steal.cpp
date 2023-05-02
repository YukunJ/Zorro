/**
 * @file local_fine_pool.cpp
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 18 2023
 *
 * This is an implementation file that implements the second version of
 * threadpool using thread-local work queue but fine locking
 */

#include "local_fine_pool_naive_steal.h"

LocalFinePoolNaiveSteal::LocalFinePoolNaiveSteal(int concurrency,
                                                 PoolType pool_type)
    : BasePool(concurrency, pool_type) {
  for (int i = 0; i < concurrency_; i++) {
    // create padded resources
    auto r = std::make_unique<PaddedResourceFine>();
    resources_.push_back(std::move(r));
  }
  for (int i = 0; i < concurrency_; i++) {
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
            { has_next_task = resources_[id]->queue.pop(next_task); }
            if (!has_next_task) {
              // steal here
              for (int j = 1; j < concurrency_; j++) {
                int steal_index = (id + j) % concurrency_;
                has_next_task = resources_[steal_index]->queue.pop(next_task);
                if (has_next_task) {
                  break;
                }
              }
              if (!has_next_task) {
                if (submit_count_ == finish_count_) {
                  cv_count_.notify_all();
                }
                std::this_thread::yield();
              }
            }
          } while (!has_next_task && status_ != PoolStatus::EXIT);

          if (!has_next_task && status_ == PoolStatus::EXIT) {
            // this pool is about to be destroyed
            return;
          }
        }
        next_task();
        // there is no add_fetch available
        int post_increment = finish_count_.fetch_add(1) + 1;
        if (post_increment == submit_count_.load()) {
          // notify the WaitUntilFinished() caller
          cv_count_.notify_all();
        }
      }
    });
  }
}

LocalFinePoolNaiveSteal::~LocalFinePoolNaiveSteal() {
  // harvest all worker threads
  for (auto& worker : threads_) {
    worker.join();
  }
}

void LocalFinePoolNaiveSteal::Submit(Task task) {
  assert(status_ != PoolStatus::EXIT);
  // Round-robin load balancer
  int robin = submit_count_.fetch_add(1);  // atomic add
  int i = robin % concurrency_;
  {
    // does this create contention? but seems unavoidable
    resources_[i]->queue.push(std::move(task));
  }
  resources_[i]->cv.notify_all();
}

void LocalFinePoolNaiveSteal::WaitUntilFinished() {
  std::unique_lock<std::mutex> lock(mtx_count_);
  cv_count_.wait(lock, [this]() -> bool {
    return submit_count_.load() == finish_count_.load();
  });
  finish_count_.store(0);
  submit_count_.store(0);
  printf("task count: %d\n", finish_count_.load());
  fflush(stdout);
}

void LocalFinePoolNaiveSteal::Exit() {
  status_ = PoolStatus::EXIT;
  for (int i = 0; i < concurrency_; i++) {
    // wake up sleeping worker
    resources_[i]->cv.notify_all();
  }
}