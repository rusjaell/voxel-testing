#include "pch.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) {
    for (size_t i = 0; i < numThreads; i++) {
        _workers.emplace_back([this] {
            Worker();
        });
    }
}

ThreadPool::~ThreadPool() 
{
    {
        std::unique_lock lock(_mutex);
        _stop = true;
    }

    _condition.notify_all();
    for (std::thread& worker : _workers) {
        worker.join();
    }
}

void ThreadPool::Worker() 
{
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock lock(_mutex);
            _condition.wait(lock, [this] {
                return _stop || !_tasks.empty();
                });
            if (_stop && _tasks.empty()) {
                return;
            }
            task = std::move(_tasks.front());
            _tasks.pop();
        }
        task();
    }
}
