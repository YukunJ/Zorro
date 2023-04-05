/**
 * @file base_pool.h
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Apr 03 2023
 *
 * This is a header file specifying the interface for
 * all types of threadpool implementations in this project
 */

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

class BasePool {
 public:
  /* requires the type specification */
  BasePool(PoolType pool_type) : pool_type_(pool_type){};

  /* as always */
  virtual ~BasePool(){};

  /*
   * Get the PoolType
   */
  auto GetType() -> PoolType { return pool_type_; }

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

  /**
   * Signal to worker threads that no more tasks will be submitted
   * They can clean up and return from the thread loop
   */
  virtual void Exit() = 0;

 protected:
  /* no copy & move allowed for all kinds of thread pool */
  BasePool(const BasePool&) = delete;
  BasePool(BasePool&&) = delete;
  BasePool& operator=(const BasePool&) = delete;
  BasePool& operator=(BasePool&&) = delete;

  PoolType pool_type_;
};
