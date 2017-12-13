#include <iostream>
#include "ThreadPool.h"
#include "Promise.h"
#include "Future.h"

template<typename T, typename F>
Future<typename std::result_of<F(T)>::type> Map(Future<T> future, const F &function) {
    using K = typename std::result_of<F(T)>::type;
    ThreadPool *currentPool = nullptr;
    if (future.getPool()) {
        currentPool = future.getPool();
    } else if (ThreadPool::localThreadPoolPtr) {
        currentPool = ThreadPool::localThreadPoolPtr;
    }
    std::shared_ptr<Promise<K>> promisePtr = std::shared_ptr<Promise<K>>(new Promise<K>());

    if (currentPool) {
        currentPool->execute([&future, &function, promisePtr] {
            promisePtr->set(std::move(function(future.get())));

        });
    } else {
        std::thread([&future, &function, promisePtr]() {
            promisePtr->set(std::move(function(future.get())));
        }).detach();
    }
    return std::move(promisePtr->getFuture());
}