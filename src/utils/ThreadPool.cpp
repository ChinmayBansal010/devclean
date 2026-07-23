#include "utils/ThreadPool.hpp"

#include <thread>

ThreadPool::ThreadPool(size_t numThreads)
{
    if (numThreads == 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0)
            numThreads = 4;
    }

    for (size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back([this]() { worker(); });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stopped = true;
    }
    condition.notify_all();

    for (auto& t : threads) {
        if (t.joinable())
            t.join();
    }
}

void ThreadPool::worker()
{
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() { return stopped || !tasks.empty(); });

            if (stopped && tasks.empty())
                break;

            if (!tasks.empty()) {
                task = std::move(tasks.front());
                tasks.pop();
            }
        }

        if (task) {
            task();
        }
    }
}
