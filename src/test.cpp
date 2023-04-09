//
// Created by leo guo on 2023-04-09.
//

#include "test.h"
#include <iostream>
#include <thread>
#include <random>

#include "dummy_pool.h"
#include "timer.h"

#define TASK_COUNT_LIGHT 100000
#define TASK_COUNT_NORMAL 100
#define TASK_COUNT_IMBALANCED 100
#define TASK_COUNT_CORRECTNESS 100000
#define THREAD_COUNT 8

// To disable optimization on light_task
void light_task() {
    return;
}


void Test::light_test() {
    DummyPool pool = DummyPool(THREAD_COUNT, PoolType::STREAM);
    Timer timer;
    for (int i = 0; i < TASK_COUNT_LIGHT; i++) {
        pool.Submit(light_task);
    }
    // TODO: might delete Exit since we need to support spawning task from task
    pool.Exit();
    pool.WaitUntilFinished();
    std::cout << "Light test: Timer has elapsed " << timer.Elapsed() << " millis time"
              << std::endl;
}

void normal_task() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    return;
}

void Test::normal_test() {
    DummyPool pool = DummyPool(THREAD_COUNT, PoolType::STREAM);
    Timer timer;
    for (int i = 0; i < TASK_COUNT_NORMAL; i++) {
        pool.Submit(normal_task);
    }
    // TODO: might delete Exit since we need to support spawning task from task
    pool.Exit();
    pool.WaitUntilFinished();
    std::cout << "Normal test: Timer has elapsed " << timer.Elapsed() << " millis time"
              << std::endl;
}

void imbalanced_task(int duration) {
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    return;
}

void Test::imbalanced_test() {
    // TODO: setup seed for generators
    // First generate the test sequence
    int durations[TASK_COUNT_IMBALANCED];
    std::default_random_engine generator_light;
    std::uniform_int_distribution<int> distribution_light(5,15);

    std::default_random_engine generator_heavy;
    std::uniform_int_distribution<int> distribution_heavy(90,110);

    int counter = 0;
    for (int i = 0; i < TASK_COUNT_IMBALANCED; i++) {
        if (i % THREAD_COUNT == 0) {
            durations[i] = distribution_heavy(generator_heavy);
        } else {
            durations[i] = distribution_light(generator_light);
        }
        counter += durations[i];
    }

    DummyPool pool = DummyPool(THREAD_COUNT, PoolType::STREAM);
    Timer timer;
    for (int i = 0; i < TASK_COUNT_IMBALANCED; i++) {
        pool.Submit(std::bind(imbalanced_task, durations[i]));
    }
    // TODO: might delete Exit since we need to support spawning task from task
    pool.Exit();
    pool.WaitUntilFinished();
    std::cout << "Imbalanced test: Timer has elapsed " << timer.Elapsed() << " millis time"
              << std::endl;
    std::cout << counter << std::endl;
}

void correctness_test_helper(int *buffer, int index) {
    buffer[index] += 1;
}

void Test::correctness_test() {
    DummyPool pool = DummyPool(THREAD_COUNT, PoolType::STREAM);
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
    std::cout << "Correctness test: Timer has elapsed " << timer.Elapsed() << " millis time"
              << std::endl;
    for (int i = 0; i < TASK_COUNT_CORRECTNESS; i++) {
        assert(buffer[i] == 1);
    }
}