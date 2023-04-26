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
#include "fine_queue.h"

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