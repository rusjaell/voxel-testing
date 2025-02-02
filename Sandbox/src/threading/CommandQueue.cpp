#include "pch.h"
#include "CommandQueue.h"

std::queue<std::function<void()>> CommandQueue::_taskQueue;
std::mutex CommandQueue::_queueMutex;
std::condition_variable CommandQueue::_condition;

void CommandQueue::Enqueue(std::function<void()> task)
{
    {
        std::lock_guard lock(_queueMutex);
        _taskQueue.push(std::move(task));
    }

    _condition.notify_one();
}

void CommandQueue::Process()
{
    while (!_taskQueue.empty())
    {
        std::function<void()> task;
        {
            std::lock_guard lock(_queueMutex);
            task = std::move(_taskQueue.front());
            _taskQueue.pop();
        }

        task();
    }
}
