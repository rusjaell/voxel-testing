#ifndef THREADPOOL_H
#define THREADPOOL_H

class ThreadPool 
{
public:
    ThreadPool(size_t numThreads = 4);
    ~ThreadPool();

    template <class F>
    std::future<void> Enqueue(F&& f)
    {
        std::shared_ptr<std::packaged_task<void()>> task = std::make_shared<std::packaged_task<void()>>(std::forward<F>(f));
        std::future<void> res = task->get_future();
        {
            std::unique_lock lock(_mutex);
            _tasks.push([task] 
            {
                (*task)(); 
            });
        }
        _condition.notify_one();
        return res;
    }


private:
    void Worker();

    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _tasks;
    std::mutex _mutex;
    std::condition_variable _condition;
    bool _stop = false;
};

#endif