/**
 * @file main.cppJ
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 03 2023
 *
 * This is the main program entry for performance benchmarking
 * run the program by './run_pool [extra parameters specification see below]'
 */

#include <cstring>
#include <iomanip>
#include <iostream>
#include <thread>

#include "dummy_pool.h"
#include "global_pool.h"
#include "local_coarse_pool.h"
#include "local_fine_pool.h"
#include "local_fine_pool_naive_steal.h"
#include "test.h"
#include "timer.h"

#define MSG "Hello World from Zorro!"

void print_formatted_vector(const std::vector<std::string>& vec,
                            const std::vector<std::string>& base_ref,
                            bool compare = true) {
  const int width = 20;
  std::cout << "|";
  for (int i = 0; i < vec.size(); i++) {
    int offset = vec[i].size();
    std::cout << vec[i];
    if (i > 0 && compare) {
      float base_time = static_cast<float>(std::stoull(base_ref[i]));
      float this_time = static_cast<float>(std::stoull(vec[i]));
      float speedup = base_time / this_time;
      char speedup_buf[10] = {0};
      snprintf(speedup_buf, 10, "%.3f", speedup);
      offset += strlen(speedup_buf) + 3;
      std::cout << " (" << speedup_buf << ")";
    }
    std::cout << std::setw(width - offset) << "|";
  }
  std::cout << std::endl;
}

int main(int argc, char* argv[]) {
  int ops = atoi(argv[1]);
  std::cout << MSG << std::endl;
  std::cout << "Benchmark: Thread Count = " << THREAD_COUNT << std::endl;

  // baseline with Dummy Pool of direct blocking execution
  std::vector<std::string> tests = {"T=" + std::to_string(THREAD_COUNT),
                                    "correctness",
                                    "light",
                                    "normal",
                                    "imbalanced",
                                    "recursion",
                                    "recursionMerge"};
  std::vector<std::string> dummy_performance{"Dummy Pool"};
  std::vector<std::string> global_performance{"Global Pool"};
  std::vector<std::string> local_coarse_performance{"Local Coarse Pool"};
  std::vector<std::string> local_fine_performance{"Local Fine Pool"};
  std::vector<std::string> naive_steal_performance{"Naive Steal"};

  // Global Pool
  if (ops == 1) {
    GlobalPool pool(THREAD_COUNT, PoolType::STREAM);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    global_performance.push_back(std::to_string(Test::correctness_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    global_performance.push_back(std::to_string(Test::light_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    global_performance.push_back(std::to_string(Test::normal_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    global_performance.push_back(std::to_string(Test::imbalanced_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    global_performance.push_back(std::to_string(Test::recursion_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    global_performance.push_back(
        std::to_string(Test::recursion_test_merge(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    pool.Exit();
  }

  // Local Coarse Pool
  if (ops == 2) {
    LocalCoarsePool pool(THREAD_COUNT, PoolType::STREAM);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    local_coarse_performance.push_back(
        std::to_string(Test::correctness_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    local_coarse_performance.push_back(std::to_string(Test::light_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    local_coarse_performance.push_back(std::to_string(Test::normal_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    local_coarse_performance.push_back(
        std::to_string(Test::imbalanced_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    local_coarse_performance.push_back(
        std::to_string(Test::recursion_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    local_coarse_performance.push_back(
        std::to_string(Test::recursion_test_merge(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    pool.Exit();
  }

  if (ops == 3)
  // Local Fine Pool
  {
    LocalFinePool pool(THREAD_COUNT, PoolType::STREAM);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    local_fine_performance.push_back(
        std::to_string(Test::correctness_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    local_fine_performance.push_back(std::to_string(Test::light_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    local_fine_performance.push_back(std::to_string(Test::normal_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    local_fine_performance.push_back(
        std::to_string(Test::imbalanced_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    local_fine_performance.push_back(
        std::to_string(Test::recursion_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    local_fine_performance.push_back(
        std::to_string(Test::recursion_test_merge(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    pool.Exit();
  }

  // Naive Steal
  if (ops == 4) {
    LocalFinePoolNaiveSteal pool(THREAD_COUNT, PoolType::STREAM);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    naive_steal_performance.push_back(
        std::to_string(Test::correctness_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    naive_steal_performance.push_back(std::to_string(Test::light_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    naive_steal_performance.push_back(std::to_string(Test::normal_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    naive_steal_performance.push_back(
        std::to_string(Test::imbalanced_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    naive_steal_performance.push_back(
        std::to_string(Test::recursion_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    naive_steal_performance.push_back(
        std::to_string(Test::recursion_test_merge(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    pool.Exit();
  }

  // Dummy Pool
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    DummyPool pool(THREAD_COUNT, PoolType::STREAM);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    dummy_performance.push_back(std::to_string(Test::correctness_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    dummy_performance.push_back(std::to_string(Test::light_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    dummy_performance.push_back(std::to_string(Test::normal_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    dummy_performance.push_back(std::to_string(Test::imbalanced_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    dummy_performance.push_back(std::to_string(Test::recursion_test(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    dummy_performance.push_back(
        std::to_string(Test::recursion_test_merge(pool)));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    pool.Exit();
  }

  std::cout << "\nPerformance Table" << std::endl;
  print_formatted_vector(tests, tests, false);
  print_formatted_vector(dummy_performance, dummy_performance, false);
  print_formatted_vector(global_performance, dummy_performance, true);
  print_formatted_vector(local_coarse_performance, dummy_performance, true);
  print_formatted_vector(local_fine_performance, dummy_performance, true);
  print_formatted_vector(naive_steal_performance, dummy_performance, true);
  return 0;
}
