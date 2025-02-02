#include "CommandQueue.h"

std::queue<std::function<void()>> CommandQueue::taskQueue;
std::mutex CommandQueue::queueMutex;
std::condition_variable CommandQueue::condition;