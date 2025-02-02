#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H

#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <iostream>

class CommandQueue
{
public:
    static void Enqueue(std::function<void()> task)
    {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            taskQueue.push(std::move(task));
        }

        condition.notify_one();
    }

    static void Process()
    {
        while (!taskQueue.empty())
        {
            std::function<void()> task;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                task = std::move(taskQueue.front());
                taskQueue.pop();
            }

            task();
        }
    }

private:
    static std::queue<std::function<void()>> taskQueue;
    static std::mutex queueMutex;
    static std::condition_variable condition;
};

#endif
