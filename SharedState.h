#pragma once

#include <condition_variable>
#include <atomic>
#include "ThreadPool.h"

template<typename>
class Promise;

template<typename>
class Future;

class State {

public:
    std::condition_variable conditionVariable;
    std::mutex mutex;
    std::exception_ptr exceptionPtr;
    std::atomic<bool> isReady;
    std::atomic<bool> hasPromise;
    ThreadPool *threadPool = nullptr;
};

template<typename T>
class FutureState : private State {
public:
    friend class Promise<T>;

    friend class Future<T>;

private:
    T value;
};

template<>
class FutureState<void> : private State {
public:
    friend class Promise<void>;

    friend class Future<void>;
};

template<typename T>
class FutureState<T &> : private State {
public:
    friend class Promise<T &>;

    friend class Future<T &>;

private:
    T *value;
};