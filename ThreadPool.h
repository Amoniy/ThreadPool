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

    static thread_local ThreadPool *thl;

    template<class F, class... Args>
    auto execute(F &&f, Args &&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()> >(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mtx);


            tasks.emplace([task]() { (*task)(); });
        }
        cond.notify_one();
        return res;
    }

    ~ThreadPool();

private:

    std::queue<std::function<void()> > tasks;
    std::vector<std::thread> threads;
    std::atomic<bool> enable;
    std::mutex mtx;
    std::condition_variable cond;


};
