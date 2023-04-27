/**
 * @file base_pool.h
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 03 2023
 *
 * This is a header file specifying the interface for
 * all types of threadpool implementations in this project
 */
#pragma once

#include <cassert>
#include <functional>

/**
 * Since Template and virtual keyword do not work well together
 * Therefore we pre-specify that the task submitted to pool
 * must be of void(void) type
 *
 * To provide argument, use std::bind or lambda
 * To get return value, pass in a reference/pointer to output position
 */
using Task = std::function<void(void)>;

/*
 * The Pool Type
 * STEAM means the worker will start working on tasks as soon as submission
 * BATCH means the worker will not start working until told so
 *
 * BATCH might be helpful in benchmarking, where we want to first submit all the
 * tasks and let the workers to start all at once
 */
enum class PoolType { STREAM, BATCH };

/*
 * The Pool current Status
 * PREPARE is in the BATCH mode when pool is not set to start running
 * RUNNING is when pool workers are actively working
 * EXIT means no more tasks will be submitted and worker could exit thread loop
 */
enum class PoolStatus { PREPARE, RUNNING, EXIT };

class BasePool {
 public:
  /* requires the thread count and type specification */
  BasePool(int concurrency, PoolType pool_type)
      : concurrency_(concurrency),
        type_(pool_type),
        status_(pool_type == PoolType::BATCH ? PoolStatus::PREPARE
                                             : PoolStatus::RUNNING){};

  /* virtual dtor as always */
  virtual ~BasePool(){};

  /*
   * Get the thread count
   */
  auto GetConcurrency() -> int { return concurrency_; }

  /*
   * Get the PoolType
   */
  auto GetType() -> PoolType { return type_; }

  /*
   * Get the PoolStatus
   */
  auto GetStatus() -> PoolStatus { return status_; }

  /**
   * Tell worker threads to begin working
   * i.e. set the status to RUNNING
   * Usually used in the BATCH mode
   */
  void Begin() {
    assert(status_ != PoolStatus::EXIT);
    status_ = PoolStatus::RUNNING;
  }

  /**
   * Signal to worker threads that no more tasks will be submitted
   * i.e. set the status to EXIT
   * They can clean up and return from the thread loop
   */
  void Exit() { status_ = PoolStatus::EXIT; }

  /* --- virtual interface to be implemented --- */
  /**
   * Submit a Task to the threadpool
   * @param task the task to be executed in threadpool
   * notice it's copied into the func scope, further
   * operations on it might use move semantics
   */
  virtual void Submit(Task task) = 0;

  /**
   * Block waiting until all the tasks submitted so far has all finished
   * Typically, should call Exit() first and then WaitUntilFinished()
   */
  virtual void WaitUntilFinished() = 0;
  /* --- end of virtual interface --- */

 protected:
  /* no copy & move allowed for all kinds of thread pool */
  BasePool(const BasePool&) = delete;
  BasePool(BasePool&&) = delete;
  BasePool& operator=(const BasePool&) = delete;
  BasePool& operator=(BasePool&&) = delete;

  int concurrency_;
  PoolType type_;
  volatile PoolStatus status_;
};
