#pragma once

#include "Future.h"
#include <thread>

template<typename T>
struct NestedTypeGetter;

template<typename T>
struct NestedTypeGetter<Future<T>> {
    typedef T type_t;
};

template<typename T>
struct NestedTypeGetter<Future<Future<T>>> {
    typedef typename NestedTypeGetter<Future<T>>::type_t type_t;
};

template<typename T>
T flattenSynchronised(const Future<T> &future) {
    return std::move(future.get());
}

template<typename T>
auto flattenSynchronised(const Future<Future<T>> &future) {
    return std::move(flattenSynchronised(std::move(future.get())));
}

template<typename T>
auto flatten(const Future<Future<T>> &future) {
    std::shared_ptr<Promise<typename NestedTypeGetter<Future<T>>::type_t>> ptr(
            new Promise<typename NestedTypeGetter<Future<T>>::type_t>());
    std::thread([ptr, &future]() {
        ptr->set(std::move(flattenSynchronised(std::move(future))));
    }).detach();
    return std::move(ptr->getFuture());
}

template<typename T>
auto flatten(Future<T> f) {
    return std::move(f);
}

template<template<typename ...> class C, typename T>
Future<C<T>> flatten(C<Future<T> > const &collection) {
    std::shared_ptr<Promise<C<T>>> ptr(new Promise<C<T>>);

    std::thread([ptr, &collection]() {
        C<T> returnCollection;
        for (auto &n: collection) {
            returnCollection.push_back(n.get());
        }
        ptr->set(returnCollection);
    }).detach();

    return ptr->getFuture();
}