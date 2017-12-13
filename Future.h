#pragma once

#include "SharedState.h"
#include <algorithm>
#include "Promise.h"

template<typename T>
class Future {

    explicit Future(std::shared_ptr<FutureState<T> > state) : state_(state), getWasUsed(false) {

    }

    void ensureInitialized() const {
        if (!state_) {
            throw std::runtime_error("Future does not have state");
        }
    }


public:

    ThreadPool *getPool() {
        return state_->threadPool;
    }

    Future(Future &&f) noexcept : state_(std::move(f.state_)), getWasUsed(f.getWasUsed.load()) {

    }

    Future &operator=(Future &&f) noexcept {
        getWasUsed = f.getWasUsed.load();
        state_ = std::move(f.state_);
        return *this;
    }

    Future(Future const &) = delete;

    Future &operator=(Future const &) = delete;


    T get() const {
        if (getWasUsed) {
            throw std::runtime_error("get() has already been used");
        }
        getWasUsed = true;
        wait();
        if (!state_->hasPromise && !isReady()) {
            throw std::runtime_error("Future does not have Promise");
        } else if (state_->exceptionPtr) {
            std::rethrow_exception(state_->exceptionPtr);
        } else {
            return std::move(state_->value);
        }
    }

    bool isReady() const {
        ensureInitialized();
        return state_->isReady;
    }

    void wait() const {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state_->mutex);
        if (isReady()) {
            return;
        }
        state_->conditionVariable.wait(lock, [this]() {
            return isReady() || !state_->hasPromise;
        });

    }

    Future() = default;

    friend class Promise<T>;

private:

    std::shared_ptr<FutureState<T> > state_;
    mutable std::atomic<bool> getWasUsed;

};

template<typename T>
class Future<T &> {

    explicit Future(std::shared_ptr<FutureState<T &> > state) : state_{state}, getWasUsed(false) {

    }

    void ensureInitialized() const {
        if (!state_) {
            throw std::runtime_error("Future does not have state");
        }
    }

public:

    Future(Future &&f) noexcept : state_(std::move(f.state_)), getWasUsed(f.getWasUsed.load()) {

    }

    Future &operator=(Future &&f) noexcept {
        getWasUsed = f.getWasUsed.load();
        state_ = std::move(f.state_);
        return *this;
    }

    Future(Future const &) = delete;

    Future &operator=(Future const &) = delete;

    T &get() const {

        if (getWasUsed) {
            throw std::runtime_error("get() has already been used.");
        }
        getWasUsed = true;
        wait();
        if (!state_->hasPromise && !isReady()) {
            throw std::runtime_error("Future does not have Promise");
        } else if (state_->exceptionPtr) {
            std::rethrow_exception(state_->exceptionPtr);
        } else
            return *state_->value;
    }

    bool isReady() const {
        ensureInitialized();
        return state_->isReady;
    }

    void wait() const {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state_->mutex);
        if (isReady()) {
            return;
        }
        state_->conditionVariable.wait(lock, [this]() {
            return isReady() || !state_->hasPromise;
        });

    }

    friend class Promise<T &>;

private:


    mutable std::atomic<bool> getWasUsed;
    std::shared_ptr<FutureState<T &> > state_;

};

template<>
class Future<void> {

    explicit Future(std::shared_ptr<FutureState<void> > state) : state_{state}, getWasUsed(false) {

    }

    void ensureInitialized() const {
        if (!state_) {
            throw std::runtime_error("Future does not have state");
        }
    }


public:


    Future(Future &&f) noexcept : state_(std::move(f.state_)), getWasUsed(f.getWasUsed.load()) {

    }

    Future &operator=(Future &&f) noexcept {
        getWasUsed = f.getWasUsed.load();
        state_ = std::move(f.state_);
        return *this;
    }

    Future(Future const &) = delete;

    Future &operator=(Future const &) = delete;

    void get() const {

        if (getWasUsed) {
            throw std::runtime_error("get() has already been used.");
        }
        getWasUsed = true;
        wait();
        if (!state_->hasPromise && !isReady()) {
            throw std::runtime_error("Future does not have Promise");
        } else if (state_->exceptionPtr) {
            std::rethrow_exception(state_->exceptionPtr);
        }
    }

    bool isReady() const {
        ensureInitialized();
        return state_->isReady;
    }

    void wait() const {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state_->mutex);
        if (isReady()) {
            return;
        }
        state_->conditionVariable.wait(lock, [this]() {
            return isReady() || !state_->hasPromise;
        });

    }

    friend class Promise<void>;

private:

    std::shared_ptr<FutureState<void> > state_;
    mutable std::atomic<bool> getWasUsed;
};

