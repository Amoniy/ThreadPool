#pragma once

#include <functional>
#include "Future.h"

template<typename T>
class Promise {
    void ensureInitialized() const {
        if (!state) {
            throw std::runtime_error("Promise does not have state");
        }
    }

public:
    void setPool(ThreadPool *threadPool) {
        state->threadPool = threadPool;
    }

    Promise() : state(std::make_shared<FutureState<T> >()), futureExists(false) {
        state->hasPromise = true;
    }

    Promise(Promise<T> &&promise) noexcept : state(std::move(promise.state)),
                                             futureExists(promise.futureExists.load()) {
        futureExists = promise.futureExists ? true : false;
    }

    ~Promise() {
        if (state) {
            state->hasPromise = false;
            state->conditionVariable.notify_one();
        }
    }

    Promise &operator=(Promise<T> &&promise) noexcept {

        futureExists = promise.futureExists.load();
        state = std::move(promise.state);
        return *this;
    };

    Promise(Promise<T> const &) = delete;

    Promise &operator=(Promise<T> const &) = delete;

    Future<T> getFuture() {
        if (futureExists) {
            throw std::runtime_error("Future already set");
        }
        futureExists = true;
        return Future<T>(state);
    }

    void set(const T &value) {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state->mutex);
        if (state->isReady) {
            throw std::runtime_error("value already set");
        }
        state->value = value;
        state->isReady = true;
        state->conditionVariable.notify_one();

    }

    void set(T &&v) {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state->mutex);
        if (state->isReady) {
            throw std::runtime_error("value already set");
        }
        state->value = std::move(v);
        state->isReady = true;
        state->conditionVariable.notify_one();
    }

    void setException(const std::exception_ptr &exceptionPtr) {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state->mutex);
        if (state->exceptionPtr) {
            throw std::runtime_error("error already set");
        }
        state->exceptionPtr = exceptionPtr;
        state->isReady = true;
        state->conditionVariable.notify_one();
    }

private:
    std::shared_ptr<FutureState<T> > state;
    std::atomic<bool> futureExists;
};

template<>
class Promise<void> {
    void ensureInitialized() const {
        if (!state) {
            throw std::runtime_error("Promise does not have state");
        }
    }

public:
    Promise()
            : state(std::make_shared<FutureState<void> >()), futureExists(false) {
        state->hasPromise = true;
    }

    ~Promise() {
        if (state) {
            state->hasPromise = false;
            state->conditionVariable.notify_one();
        }
    }

    Promise(Promise<void> &&promise) noexcept : state(std::move(promise.state)),
                                                futureExists(promise.futureExists.load()) {
    }

    Promise &operator=(Promise<void> &&promise) noexcept {
        futureExists = promise.futureExists.load();
        state = std::move(promise.state);
        return *this;
    };

    Promise(Promise<void> const &) = delete;

    Promise &operator=(Promise<void> const &) = delete;

    Future<void> getFuture() {
        if (futureExists) {
            throw std::runtime_error("Future already set");
        }
        futureExists = true;
        return Future<void>(state);
    }

    void set() {
        ensureInitialized();
        if (state->isReady) {
            throw std::runtime_error("value already set");
        }
        state->isReady = true;
        state->conditionVariable.notify_one();
    }

    void setException(const std::exception_ptr &exceptionPtr) {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state->mutex);
        if (state->exceptionPtr) {
            throw std::runtime_error("error already set");
        }
        state->exceptionPtr = exceptionPtr;
        state->isReady = true;
        state->conditionVariable.notify_one();
    };

private:
    std::shared_ptr<FutureState<void> > state;
    std::atomic<bool> futureExists;
};

template<typename T>
class Promise<T &> {
    void ensureInitialized() const {
        if (!state) {
            throw std::runtime_error("Promise does not have state");
        }
    }

public:
    Promise()
            : state(std::make_shared<FutureState<T &> >()), futureExists(false) {
        state->hasPromise = true;
    }

    ~Promise() {
        if (state) {
            state->hasPromise = false;
            state->conditionVariable.notify_one();
        }
    }

    Promise(Promise &&promise) noexcept : state(std::move(promise.state)), futureExists(promise.futureExists.load()) {

    }

    Promise &operator=(Promise &&promise) noexcept {
        futureExists = promise.futureExists.load();
        state = std::move(promise.state);
        return *this;
    };

    Promise(const Promise &) = delete;

    Promise &operator=(const Promise &) = delete;

    Future<T &> getFuture() {
        if (futureExists) {
            throw std::runtime_error("Future already set");
        }
        futureExists = true;
        return Future<T &>(state);
    }

    void set(T &v) {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state->mutex);
        if (state->isReady) {
            throw std::runtime_error("value already set");
        }
        state->value = &v;
        state->isReady = true;
        state->conditionVariable.notify_one();
    }

    void setException(const std::exception_ptr &exceptionPtr) {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state->mutex);
        if (state->exceptionPtr) {
            throw std::runtime_error("error already set");
        }
        state->exceptionPtr = exceptionPtr;
        state->isReady = true;
        state->conditionVariable.notify_one();
    }


private:
    std::shared_ptr<FutureState<T &> > state;
    std::atomic<bool> futureExists;
};