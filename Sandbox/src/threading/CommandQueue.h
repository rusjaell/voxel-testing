#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H

class CommandQueue
{
public:
    static void Enqueue(std::function<void()> task);
    static void Process();

private:
    static std::queue<std::function<void()>> _taskQueue;
    static std::mutex _queueMutex;
    static std::condition_variable _condition;
};

#endif
