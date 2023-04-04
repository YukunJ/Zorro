/**
 * @file timer.cpp
 * @expectation this implementation file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 04 2023
 *
 * This is an implementation file for the Timer utility
 */

#include "timer.h"

#include <chrono>

/* upon ctor, start_ is initialized to current time */
Timer::Timer() noexcept : start_(NowInMills()){};

/* refresh start_ to current time */
void Timer::Reset() noexcept { start_ = NowInMills(); }

/* how long has elapsed since start_ in milliseconds */
auto Timer::Elapsed() noexcept -> uint64_t { return NowInMills() - start_; }

/* utils to get current time in milliseconds */
auto Timer::NowInMills() noexcept -> uint64_t {
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  return static_cast<uint64_t>(millis);
}
