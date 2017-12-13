#pragma once

#include "SharedState.h"
#include <algorithm>
#include "Promise.h"

template<typename T>
class Future {
    explicit Future(std::shared_ptr<FutureState<T> > state) : state(state), wasUsed(false) {
    }

    void ensureInitialized() const {
        if (!state) {
            throw std::runtime_error("Future does not have state");
        }
    }


public:
    ThreadPool *getPool() {
        return state->threadPool;
    }

    Future(Future &&future) noexcept : state(std::move(future.state)), wasUsed(future.wasUsed.load()) {

    }

    Future &operator=(Future &&future) noexcept {
        wasUsed = future.wasUsed.load();
        state = std::move(future.state);
        return *this;
    }

    Future(Future const &) = delete;

    Future &operator=(Future const &) = delete;

    T get() const {
        if (wasUsed) {
            throw std::runtime_error("get() has already been used");
        }
        wasUsed = true;
        wait();
        if (!state->hasPromise && !isReady()) {
            throw std::runtime_error("Future does not have Promise");
        } else if (state->exceptionPtr) {
            std::rethrow_exception(state->exceptionPtr);
        } else {
            return std::move(state->value);
        }
    }

    bool isReady() const {
        ensureInitialized();
        return state->isReady;
    }

    void wait() const {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state->mutex);
        if (isReady()) {
            return;
        }
        state->conditionVariable.wait(lock, [this]() {
            return isReady() || !state->hasPromise;
        });
    }

    Future() = default;

    friend class Promise<T>;

private:
    std::shared_ptr<FutureState<T> > state;
    mutable std::atomic<bool> wasUsed;
};

template<typename T>
class Future<T &> {
    explicit Future(std::shared_ptr<FutureState<T &> > state) : state{state}, wasUsed(false) {
    }

    void ensureInitialized() const {
        if (!state) {
            throw std::runtime_error("Future does not have state");
        }
    }

public:
    Future(Future &&f) noexcept : state(std::move(f.state)), wasUsed(f.wasUsed.load()) {

    }

    Future &operator=(Future &&future) noexcept {
        wasUsed = future.wasUsed.load();
        state = std::move(future.state);
        return *this;
    }

    Future(Future const &) = delete;

    Future &operator=(Future const &) = delete;

    T &get() const {
        if (wasUsed) {
            throw std::runtime_error("get() has already been used.");
        }
        wasUsed = true;
        wait();
        if (!state->hasPromise && !isReady()) {
            throw std::runtime_error("Future does not have Promise");
        } else if (state->exceptionPtr) {
            std::rethrow_exception(state->exceptionPtr);
        } else
            return *state->value;
    }

    bool isReady() const {
        ensureInitialized();
        return state->isReady;
    }

    void wait() const {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state->mutex);
        if (isReady()) {
            return;
        }
        state->conditionVariable.wait(lock, [this]() {
            return isReady() || !state->hasPromise;
        });
    }

    friend class Promise<T &>;

private:
    mutable std::atomic<bool> wasUsed;
    std::shared_ptr<FutureState<T &> > state;
};

template<>
class Future<void> {
    explicit Future(std::shared_ptr<FutureState<void> > state) : state{state}, wasUsed(false) {

    }

    void ensureInitialized() const {
        if (!state) {
            throw std::runtime_error("Future does not have state");
        }
    }

public:
    Future(Future &&f) noexcept : state(std::move(f.state)), wasUsed(f.wasUsed.load()) {

    }

    Future &operator=(Future &&f) noexcept {
        wasUsed = f.wasUsed.load();
        state = std::move(f.state);
        return *this;
    }

    Future(Future const &) = delete;

    Future &operator=(Future const &) = delete;

    void get() const {
        if (wasUsed) {
            throw std::runtime_error("get() has already been used.");
        }
        wasUsed = true;
        wait();
        if (!state->hasPromise && !isReady()) {
            throw std::runtime_error("Future does not have Promise");
        } else if (state->exceptionPtr) {
            std::rethrow_exception(state->exceptionPtr);
        }
    }

    bool isReady() const {
        ensureInitialized();
        return state->isReady;
    }

    void wait() const {
        ensureInitialized();
        std::unique_lock<std::mutex> lock(state->mutex);
        if (isReady()) {
            return;
        }
        state->conditionVariable.wait(lock, [this]() {
            return isReady() || !state->hasPromise;
        });

    }

    friend class Promise<void>;

private:

    std::shared_ptr<FutureState<void> > state;
    mutable std::atomic<bool> wasUsed;
};

