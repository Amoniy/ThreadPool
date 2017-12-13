#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <memory>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>

class ThreadPool {
public :
    ThreadPool(size_t num_threads);

    static thread_local ThreadPool *localThreadPoolPtr;

    void execute(std::function<void()> const &task) {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(task);
        conditionVariable.notify_one();
    }

    ~ThreadPool();

private:
    std::queue<std::function<void()> > queue;
    std::vector<std::thread> threads;
    std::atomic<bool> working;
    std::mutex mutex;
    std::condition_variable conditionVariable;
};