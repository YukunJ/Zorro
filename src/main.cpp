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
#include "test.h"
#include "timer.h"

#define MSG "Hello World from Zorro!"

int main(int argc, char* argv[]) {
  std::cout << MSG << std::endl;
  Timer t;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "Timer has elapsed " << t.Elapsed() << " millis time"
            << std::endl;

  // baseline with Dummy Pool of direct blocking execution
  {
    DummyPool pool(THREAD_COUNT, PoolType::STREAM);
    Test::light_test(pool);
    Test::normal_test(pool);
    Test::imbalanced_test(pool);
    Test::correctness_test(pool);
  }
  return 0;
}