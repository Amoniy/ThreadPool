#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t num_thread) : working(false) {
    for (size_t i = 0; i < num_thread; i++) {
        threads.emplace_back([this]() {
            localThreadPoolPtr = this;
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    conditionVariable.wait(lock, [this]() {
                        return !queue.empty() || working;
                    });
                    if (working && queue.empty()) return;
                    task = std::move(queue.front());
                    queue.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        working = true;
        std::unique_lock<std::mutex> lock(mutex);
    }
    conditionVariable.notify_all();

    for (auto &thread: threads) {
        thread.join();
    }
}

thread_local ThreadPool *ThreadPool::localThreadPoolPtr = nullptr;