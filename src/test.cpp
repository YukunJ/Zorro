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

void Test::light_test(BasePool& pool) {
  Timer timer;
  for (int i = 0; i < TASK_COUNT_LIGHT; i++) {
    pool.Submit(light_task);
  }
  // TODO: might delete Exit since we need to support spawning task from task
  pool.Exit();
  pool.WaitUntilFinished();
  std::cout << "Light test: Timer has elapsed " << timer.Elapsed()
            << " millis time" << std::endl;
}

void normal_task() {
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  return;
}

void Test::normal_test(BasePool& pool) {
  Timer timer;
  for (int i = 0; i < TASK_COUNT_NORMAL; i++) {
    pool.Submit(normal_task);
  }
  // TODO: might delete Exit since we need to support spawning task from task
  pool.Exit();
  pool.WaitUntilFinished();
  std::cout << "Normal test: Timer has elapsed " << timer.Elapsed()
            << " millis time" << std::endl;
}

void imbalanced_task(int duration) {
  std::this_thread::sleep_for(std::chrono::milliseconds(duration));
  return;
}

void Test::imbalanced_test(BasePool& pool) {
  // TODO: setup seed for generators
  // First generate the test sequence
  int durations[TASK_COUNT_IMBALANCED];
  std::default_random_engine generator_light;
  std::uniform_int_distribution<int> distribution_light(5, 15);

  std::default_random_engine generator_heavy;
  std::uniform_int_distribution<int> distribution_heavy(90, 110);

  int counter = 0;
  for (int i = 0; i < TASK_COUNT_IMBALANCED; i++) {
    if (i % THREAD_COUNT == 0) {
      durations[i] = distribution_heavy(generator_heavy);
    } else {
      durations[i] = distribution_light(generator_light);
    }
    counter += durations[i];
  }

  Timer timer;
  for (int i = 0; i < TASK_COUNT_IMBALANCED; i++) {
    pool.Submit(std::bind(imbalanced_task, durations[i]));
  }
  // TODO: might delete Exit since we need to support spawning task from task
  pool.Exit();
  pool.WaitUntilFinished();
  std::cout << "Imbalanced test: Timer has elapsed " << timer.Elapsed()
            << " millis time" << std::endl;
  // std::cout << counter << std::endl;
}

void correctness_test_helper(int* buffer, int index) { buffer[index] += 1; }

void Test::correctness_test(BasePool& pool) {
  int buffer[TASK_COUNT_CORRECTNESS];
  for (int i = 0; i < TASK_COUNT_CORRECTNESS; i++) {
    buffer[i] = 0;
  }
  Timer timer;
  for (int i = 0; i < TASK_COUNT_CORRECTNESS; i++) {
    pool.Submit(std::bind(correctness_test_helper, buffer, i));
  }
  // TODO: might delete Exit since we need to support spawning task from task
  pool.Exit();
  pool.WaitUntilFinished();
  std::cout << "Correctness test: Timer has elapsed " << timer.Elapsed()
            << " millis time" << std::endl;
  for (int i = 0; i < TASK_COUNT_CORRECTNESS; i++) {
    assert(buffer[i] == 1);
  }
}

int partition(int arr[], int start, int end) {
    int pivot = arr[start];
    int count = 0;
    for (int i = start + 1; i <= end; i++) {
        if (arr[i] <= pivot)
            count++;
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
    //if (start >= end)
        //return;
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


void Test::recursion_test(BasePool& pool) {
    long counter = 0;
    int Rand[ARRAY_SIZE_RECURSION];
    for (int i = 0; i <ARRAY_SIZE_RECURSION; i++) {
        Rand[i] = rand();
        counter += Rand[i];
    }
    Timer timer;
    pool.Submit(std::bind(quickSort, Rand, 0, ARRAY_SIZE_RECURSION - 1, &pool));
    pool.WaitUntilFinished();
    std::cout << "Recursion test: Timer has elapsed " << timer.Elapsed()
              << " millis time" << std::endl;
    long new_counter = Rand[0];
    for (int i = 0; i < ARRAY_SIZE_RECURSION - 1; i++) {
        assert(Rand[i] <= Rand[i + 1]);
        new_counter += Rand[i + 1];
    }
    assert(counter == new_counter);
}