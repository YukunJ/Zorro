/**
 * @file main.cppJ
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 03 2023
 *
 * This is the main program entry for performance benchmarking
 * run the program by './run_pool [extra parameters specification see below]'
 */

#include <iostream>
#include <thread>

#include "dummy_pool.h"
#include "global_pool.h"
#include "test.h"
#include "timer.h"

#define MSG "Hello World from Zorro!"

int main(int argc, char* argv[]) {
  std::cout << MSG << std::endl;
  std::cout << "Benchmark: Thread Count = " << THREAD_COUNT << std::endl;

  // baseline with Dummy Pool of direct blocking execution
  std::cout << "==========Dummy Pool============" << std::endl;
  {
    DummyPool pool(THREAD_COUNT, PoolType::STREAM);
    Test::correctness_test(pool);
  }
  {
    DummyPool pool(THREAD_COUNT, PoolType::STREAM);
    Test::light_test(pool);
  }
  {
    DummyPool pool(THREAD_COUNT, PoolType::STREAM);
    Test::normal_test(pool);
  }
  {
    DummyPool pool(THREAD_COUNT, PoolType::STREAM);
    Test::imbalanced_test(pool);
  }

  std::cout << "\n==========Global Pool============" << std::endl;
  {
    GlobalPool pool(THREAD_COUNT, PoolType::STREAM);
    Test::correctness_test(pool);
  }
  {
    GlobalPool pool(THREAD_COUNT, PoolType::STREAM);
    Test::light_test(pool);
  }
  {
    GlobalPool pool(THREAD_COUNT, PoolType::STREAM);
    Test::normal_test(pool);
  }
  {
    GlobalPool pool(THREAD_COUNT, PoolType::STREAM);
    Test::imbalanced_test(pool);
  }
  return 0;
}