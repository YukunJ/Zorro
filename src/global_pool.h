/**
 * @file global_pool.h
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 10 2023
 *
 * This is a header file that specifies the first version of threadpool
 * using a global work queue and each worker polling from this single queue
 * for more task
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "base_pool.h"

class GlobalPool final : public BasePool {
 public:
  GlobalPool(int concurrency, PoolType pool_type);

  ~GlobalPool();

  void Submit(Task task) override;

  void WaitUntilFinished() override;

  void Exit();

 private:
  std::atomic<int> submit_count_{0};
  std::atomic<int> finish_count_{0};

  std::vector<std::thread> threads_;
  std::queue<Task> task_queue_;
  std::mutex mtx_;
  std::condition_variable cv_;

  std::mutex mtx_count_;
  std::condition_variable cv_count_;
};