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

#include "base_pool.h"
#include "timer.h"
#include "test.h"

#define MSG "Hello World from Zorro!"

int main(int argc, char* argv[]) {
  std::cout << MSG << std::endl;
  Timer t;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "Timer has elapsed " << t.Elapsed() << " millis time"
            << std::endl;
  return 0;
}