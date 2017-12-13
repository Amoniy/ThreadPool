#pragma once

#include <functional>
#include "Future.h"

template<typename T>
class Promise {
    void ensureInitialized() const {
        if (!state_) {
            throw std::runtime_error("Promise does not have state");
        }
    }

public:
    void setPool(ThreadPool *threadPool) {
        state_->threadPool = threadPool;
    }

    Promise() : state_(std::make_shared<FutureState<T> >()), futureExists_(false) {
        state_->hasPromise = true;
    }

    Promise(Promise<T> &&promise) noexcept : state_(std::move(promise.state_)),
                                             futureExists_(promise.futureExists_.load()) {
        futureExists_ = promise.futureExists_ ? true : false;
    }

    ~Promise() {
        if (state_) {
            state_->hasPromise = false;
            state_->conditionVariable.notify_one();
        }
    }

    Promise &operator=(Promise<T> &&promise) noexcept {

        futureExists_ = promise.futureExists_.load();
        state_ = std::move(promise.state_);
        return *this;
    };

    Promise(Promise<T> const &) = delete;

    Promise &operator=(Promise<T> const &) = delete;

    Future<T> getFuture() {
        if (futureExists_) {
            throw std::runtime_error("Future already set");
        }
        futureExists_ = true;
        return Future<T>(state_);
    }

    void set(const T &v) {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state_->mutex);
        if (state_->isReady) {
            throw std::runtime_error("value already set");
        }
        state_->value = v;
        state_->isReady = true;
        state_->conditionVariable.notify_one();

    }

    void set(T &&v) {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state_->mutex);
        if (state_->isReady) {
            throw std::runtime_error("value already set");
        }
        state_->value = std::move(v);
        state_->isReady = true;
        state_->conditionVariable.notify_one();
    }

    void setException(const std::exception_ptr &e) {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state_->mutex);
        if (state_->exceptionPtr) {
            throw std::runtime_error("error already set");
        }
        state_->exceptionPtr = e;
        state_->isReady = true;
        state_->conditionVariable.notify_one();
    }

private:
    std::shared_ptr<FutureState<T> > state_;
    std::atomic<bool> futureExists_;
};

template<>
class Promise<void> {
    void ensureInitialized() const {
        if (!state_) {
            throw std::runtime_error("Promise does not have state");
        }
    }

public:
    Promise()
            : state_(std::make_shared<FutureState<void> >()), futureExists_(false) {
        state_->hasPromise = true;
    }

    ~Promise() {
        if (state_) {
            state_->hasPromise = false;
            state_->conditionVariable.notify_one();
        }
    }

    Promise(Promise<void> &&promise) noexcept : state_(std::move(promise.state_)),
                                                futureExists_(promise.futureExists_.load()) {
    }

    Promise &operator=(Promise<void> &&promise) noexcept {
        futureExists_ = promise.futureExists_.load();
        state_ = std::move(promise.state_);
        return *this;
    };

    Promise(Promise<void> const &) = delete;

    Promise &operator=(Promise<void> const &) = delete;

    Future<void> getFuture() {
        if (futureExists_) {
            throw std::runtime_error("Future already set");
        }
        futureExists_ = true;
        return Future<void>(state_);
    }

    void set() {
        ensureInitialized();
        if (state_->isReady) {
            throw std::runtime_error("value already set");
        }
        state_->isReady = true;
        state_->conditionVariable.notify_one();
    }

    void setException(const std::exception_ptr &e) {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state_->mutex);
        if (state_->exceptionPtr) {
            throw std::runtime_error("error already set");
        }
        state_->exceptionPtr = e;
        state_->isReady = true;
        state_->conditionVariable.notify_one();
    };

private:
    std::shared_ptr<FutureState<void> > state_;
    std::atomic<bool> futureExists_;
};

template<typename T>
class Promise<T &> {
    void ensureInitialized() const {
        if (!state_) {
            throw std::runtime_error("Promise does not have state");
        }
    }

public:
    Promise()
            : state_(std::make_shared<FutureState<T &> >()), futureExists_(false) {
        state_->hasPromise = true;
    }

    ~Promise() {
        if (state_) {
            state_->hasPromise = false;
            state_->conditionVariable.notify_one();
        }
    }

    Promise(Promise &&promise) noexcept : state_(std::move(promise.state_)),
                                          futureExists_(promise.futureExists_.load()) {
    }

    Promise &operator=(Promise &&promise) noexcept {
        futureExists_ = promise.futureExists_.load();
        state_ = std::move(promise.state_);
        return *this;
    };

    Promise(const Promise &) = delete;

    Promise &operator=(const Promise &) = delete;

    Future<T &> getFuture() {
        if (futureExists_) {
            throw std::runtime_error("Future already set");
        }
        futureExists_ = true;
        return Future<T &>(state_);
    }

    void set(T &v) {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state_->mutex);
        if (state_->isReady) {
            throw std::runtime_error("value already set");
        }
        state_->value = &v;
        state_->isReady = true;
        state_->conditionVariable.notify_one();

    }

    void setException(const std::exception_ptr &e) {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state_->mutex);
        if (state_->exceptionPtr) {
            throw std::runtime_error("error already set");
        }
        state_->exceptionPtr = e;
        state_->isReady = true;
        state_->conditionVariable.notify_one();
    }


private:
    std::shared_ptr<FutureState<T &> > state_;
    std::atomic<bool> futureExists_;

};