/**
 * @file timer.h
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 04 2023
 *
 * This is a header file for the Timer utility
 */
#pragma once

#include <cstdint>

/*
 * Utility class for taking the time duration
 */
class Timer {
 public:
  /* upon ctor, start_ is initialized to current time */
  Timer() noexcept;

  /* refresh start_ to current time */
  void Reset() noexcept;

  /* how long has elapsed since start_ in milliseconds */
  auto Elapsed() noexcept -> uint64_t;

 private:
  /* utils to get current time in milliseconds */
  auto NowInMills() noexcept -> uint64_t;
  uint64_t start_;
};
