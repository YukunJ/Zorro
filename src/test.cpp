//
// Created by leo guo on 2023-04-09.
//

#include "test.h"

#include <iostream>
#include <random>
#include <thread>

#include "dummy_pool.h"
#include "timer.h"

// To disable optimization on light_task
void light_task() { return; }

uint64_t Test::light_test(BasePool &pool) {
  Timer timer;
  for (int i = 0; i < TASK_COUNT_LIGHT; i++) {
    pool.Submit(light_task);
  }
  pool.WaitUntilFinished();
  uint64_t result = timer.Elapsed();
  std::cout << "Light test: Timer has elapsed " << result << " millis time"
            << std::endl;
  return result;
}

void normal_task() {
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  return;
}

uint64_t Test::normal_test(BasePool &pool) {
  Timer timer;
  for (int i = 0; i < TASK_COUNT_NORMAL; i++) {
    pool.Submit(normal_task);
  }
  pool.WaitUntilFinished();
  uint64_t result = timer.Elapsed();
  std::cout << "Normal test: Timer has elapsed " << result << " millis time"
            << std::endl;
  return result;
}

void imbalanced_task(int duration) {
  std::this_thread::sleep_for(std::chrono::milliseconds(duration));
  return;
}

uint64_t Test::imbalanced_test(BasePool &pool) {
  // TODO: setup seed for generators
  // First generate the test sequence
  int durations[TASK_COUNT_IMBALANCED];
  std::default_random_engine generator_light;
  std::uniform_int_distribution<int> distribution_light(5, 15);

  std::default_random_engine generator_heavy;
  std::uniform_int_distribution<int> distribution_heavy(90, 110);

  for (int i = 0; i < TASK_COUNT_IMBALANCED; i++) {
    if (i % THREAD_COUNT == 0) {
      durations[i] = distribution_heavy(generator_heavy);
    } else {
      durations[i] = distribution_light(generator_light);
    }
  }

  Timer timer;
  for (int i = 0; i < TASK_COUNT_IMBALANCED; i++) {
    pool.Submit(std::bind(imbalanced_task, durations[i]));
  }
  pool.WaitUntilFinished();
  uint64_t result = timer.Elapsed();
  std::cout << "Imbalanced test: Timer has elapsed " << result << " millis time"
            << std::endl;
  return result;
}

void correctness_test_helper(int *buffer, int index) { buffer[index] += 1; }

uint64_t Test::correctness_test(BasePool &pool) {
  int buffer[TASK_COUNT_CORRECTNESS];
  for (int i = 0; i < TASK_COUNT_CORRECTNESS; i++) {
    buffer[i] = 0;
  }
  Timer timer;
  for (int i = 0; i < TASK_COUNT_CORRECTNESS; i++) {
    pool.Submit(std::bind(correctness_test_helper, buffer, i));
  }
  pool.WaitUntilFinished();
  uint64_t result = timer.Elapsed();
  std::cout << "Correctness test: Timer has elapsed " << result
            << " millis time" << std::endl;
  for (int i = 0; i < TASK_COUNT_CORRECTNESS; i++) {
    assert(buffer[i] == 1);
  }
  return result;
}

int partition(int arr[], int start, int end) {
  int pivot = arr[start];
  int count = 0;
  for (int i = start + 1; i <= end; i++) {
    if (arr[i] <= pivot) count++;
  }

  // Giving pivot element its correct position
  int pivotIndex = start + count;
  std::swap(arr[pivotIndex], arr[start]);
  // Sorting left and right parts of the pivot element
  int i = start, j = end;
  while (i < pivotIndex && j > pivotIndex) {
    while (arr[i] <= pivot) {
      i++;
    }
    while (arr[j] > pivot) {
      j--;
    }
    if (i < pivotIndex && j > pivotIndex) {
      std::swap(arr[i++], arr[j--]);
    }
  }
  return pivotIndex;
}

void quickSort(int arr[], int start, int end, BasePool *pool) {
  // base case
  // if (start >= end)
  // return;
  if (end - start <= QUICK_SORT_THRESHOLD) {
    std::sort(arr + start, arr + end + 1);
    return;
  }
  // partitioning the array
  int p = partition(arr, start, end);
  // Sorting the left part
  pool->Submit(std::bind(quickSort, arr, start, p - 1, pool));
  // Sorting the right part
  pool->Submit(std::bind(quickSort, arr, p + 1, end, pool));
}

uint64_t Test::recursion_test(BasePool &pool) {
  long counter = 0;
  int Rand[ARRAY_SIZE_RECURSION];
  for (int i = 0; i < ARRAY_SIZE_RECURSION; i++) {
    Rand[i] = rand();
    counter += Rand[i];
  }
  Timer timer;
  pool.Submit(std::bind(quickSort, Rand, 0, ARRAY_SIZE_RECURSION - 1, &pool));
  pool.WaitUntilFinished();
  uint64_t result = timer.Elapsed();
  std::cout << "Recursion test (quick sort): Timer has elapsed " << result
            << " millis time" << std::endl;
  long new_counter = Rand[0];
  for (int i = 0; i < ARRAY_SIZE_RECURSION - 1; i++) {
    assert(Rand[i] <= Rand[i + 1]);
    new_counter += Rand[i + 1];
  }
  assert(counter == new_counter);
  return result;
}

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
// Inplace Implementation
void merge(int arr[], int start, int mid, int end, std::atomic<int> *flag,
           std::atomic<int> *left_flag, std::atomic<int> *right_flag,
           BasePool *pool) {
  if (*left_flag == 0 || *right_flag == 0) {
    pool->Submit(std::bind(merge, arr, start, mid, end, flag, left_flag,
                           right_flag, pool));
    return;
  }
  delete left_flag;
  delete right_flag;
  int start2 = mid + 1;

  // If the direct merge is already sorted
  if (arr[mid] <= arr[start2]) {
    return;
  }

  // Two pointers to maintain start
  // of both arrays to merge
  while (start <= mid && start2 <= end) {
    // If element 1 is in right place
    if (arr[start] <= arr[start2]) {
      start++;
    } else {
      int value = arr[start2];
      int index = start2;

      // Shift all the elements between element 1
      // element 2, right by 1.
      while (index != start) {
        arr[index] = arr[index - 1];
        index--;
      }
      arr[start] = value;

      // Update all the pointers
      start++;
      mid++;
      start2++;
    }
  }
  flag->fetch_add(1);
}

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r, BasePool *pool,
               std::atomic<int> *flag) {
  if (r - l <= MERGE_SORT_THRESHOLD) {
    std::sort(arr + l, arr + r + 1);
    flag->fetch_add(1);
    return;
  } else if (l < r) {
    // Same as (l + r) / 2, but avoids overflow
    // for large l and r
    int m = l + (r - l) / 2;
    // Sort first and second halves
    std::atomic<int> *left_flag = new std::atomic<int>(0);
    pool->Submit(std::bind(mergeSort, arr, l, m, pool, left_flag));
    std::atomic<int> *right_flag = new std::atomic<int>(0);
    pool->Submit(std::bind(mergeSort, arr, m + 1, r, pool, right_flag));
    pool->Submit(
        std::bind(merge, arr, l, m, r, flag, left_flag, right_flag, pool));
  }
}

uint64_t Test::recursion_test_merge(BasePool &pool) {
  long counter = 0;
  int Rand[ARRAY_SIZE_RECURSION_MERGE];
  for (int i = 0; i < ARRAY_SIZE_RECURSION_MERGE; i++) {
    Rand[i] = rand();
    counter += Rand[i];
  }
  Timer timer;
  std::atomic<int> *flag = new std::atomic<int>(0);
  pool.Submit(std::bind(mergeSort, Rand, 0, ARRAY_SIZE_RECURSION_MERGE - 1,
                        &pool, flag));
  pool.WaitUntilFinished();
  uint64_t result = timer.Elapsed();
  std::cout << "Recursion test (merge sort): Timer has elapsed " << result
            << " millis time" << std::endl;
  long new_counter = Rand[0];
  for (int i = 0; i < ARRAY_SIZE_RECURSION_MERGE - 1; i++) {
    // std::cout << i << " " << Rand[i] << " " << i + 1 << " " << Rand[i + 1] <<
    // std::endl;
    assert(Rand[i] <= Rand[i + 1]);
    new_counter += Rand[i + 1];
  }
  assert(counter == new_counter);
  return result;
}