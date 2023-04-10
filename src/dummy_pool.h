/**
 * @file dummy_pool.h
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 10 2023
 *
 * This is a header file that implements the Dummy ThreadPool
 * which directly blockingly execute the task upon submission
 * this mainly serves as the baseline in benchmarking
 */

#pragma once

#include "base_pool.h"

class DummyPool : public BasePool {
 public:
  DummyPool(int concurrency, PoolType pool_type)
      : BasePool(concurrency, pool_type){};

  void Submit(Task task) override { task(); }

  void WaitUntilFinished() override {}
};