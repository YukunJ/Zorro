/**
 * @file local_coarse_pool.h
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 18 2023
 *
 * This is a header file that specifies the second version of threadpool
 * which is thread-local work queue with coarse locking
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "base_pool.h"

/* padded this struct to be at least multiples of cache-line width to avoid
 * false-sharing */
typedef struct PaddedResource {
  std::queue<Task> queue;
  std::mutex mtx;
  std::condition_variable cv;
} PaddedResource __attribute__((aligned(256)));

class LocalCoarsePool final : public BasePool {
 public:
  LocalCoarsePool(int concurrency, PoolType pool_type);

  ~LocalCoarsePool();

  void Submit(Task task) override;

  void WaitUntilFinished() override;

  void Exit();

 private:
  std::atomic<int> submit_count_{0};
  std::atomic<int> finish_count_{0};
  std::vector<std::thread> threads_;
  std::vector<std::unique_ptr<PaddedResource>> resources_;
  std::mutex mtx_count_;
  std::condition_variable cv_count_;
};