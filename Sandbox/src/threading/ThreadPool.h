#include <queue>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <future>
#include <iostream>

class ThreadPool
{
public:
    ThreadPool(size_t numThreads = 4)
    {
        for (size_t i = 0; i < numThreads; i++){
            workers.emplace_back([this] {
                Worker();
            });
        }
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }

        condition.notify_all();
        for (auto& worker : workers) {
            worker.join();
        }
    }

    template <class F>
    std::future<void> Enqueue(F&& f)
    {
        auto task = std::make_shared<std::packaged_task<void()>>(std::forward<F>(f));
        std::future<void> res = task->get_future();

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.push([task] { 

                (*task)();
            });
        }

        condition.notify_one();
        return res;
    }

private:
    void Worker()
    {
        while (true)
        {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(queueMutex);

                condition.wait(lock, [this] {
                    return stop || !tasks.empty();
                });

                if (stop && tasks.empty()) {
                    return;
                }

                task = std::move(tasks.front());
                tasks.pop();
            }

            task();
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;
};