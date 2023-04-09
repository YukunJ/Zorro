#pragma once

#include <cassert>
#include <functional>
#include "base_pool.h"

class DummyPool : public BasePool {
public:
    /* requires the thread count and type specification */
    DummyPool(int concurrency, PoolType pool_type) : BasePool(concurrency, pool_type){};

    void Submit(Task task) override {
        task();
    }

    void WaitUntilFinished() override {}

    /* no copy & move allowed for all kinds of thread pool */
    DummyPool(const DummyPool&) = delete;
    DummyPool(DummyPool&&) = delete;
    DummyPool& operator = (const DummyPool&) = delete;
    DummyPool& operator = (DummyPool&&) = delete;
};