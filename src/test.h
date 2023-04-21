//
// Created by leo guo on 2023-04-09.
//

#ifndef SRC_TEST_H
#define SRC_TEST_H

#include <cstdint>

#include "base_pool.h"
#define TASK_COUNT_LIGHT 100000
#define TASK_COUNT_NORMAL 100
#define TASK_COUNT_IMBALANCED 100
#define TASK_COUNT_CORRECTNESS 100000
#define ARRAY_SIZE_RECURSION 1000000
#define QUICK_SORT_THRESHOLD 1000
#define ARRAY_SIZE_RECURSION_MERGE 100000
#define MERGE_SORT_THRESHOLD 1000

constexpr static int THREAD_COUNT = 8;

class Test {
 public:
  static uint64_t light_test(BasePool& pool);
  static uint64_t normal_test(BasePool& pool);
  static uint64_t correctness_test(BasePool& pool);
  static uint64_t imbalanced_test(BasePool& pool);
  static uint64_t recursion_test(BasePool& pool);
  static uint64_t recursion_test_merge(BasePool& pool);
};

#endif  // SRC_TEST_H
