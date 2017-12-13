#include "Promise.h"
#include "Future.h"
#include <tuple>

template<typename T, typename Q = void>
struct NestedTypeGetter;

template<typename T>
struct NestedTypeGetter<T> {
    typedef T type_t;
};

template<typename T>
struct NestedTypeGetter<Future<T>> {
    typedef typename NestedTypeGetter<T>::type_t type_t;
};

template<typename ...Args>
struct NestedTypeGetter<std::tuple<Args...>> {
    typedef std::tuple<typename NestedTypeGetter<Args>::type_t...> type_t;
};

template<typename T>
T flatten(const T &future) {
    return std::move(future);
}

template<typename T>
auto flatten(const Future<T> &future) {
    return std::move(flatten(std::move(future.get())));
}

template<typename ...Args, std::size_t... I>
auto flatten(const std::tuple<Args...> &tuple, std::index_sequence<I...>) {
    return std::make_tuple(flatten(std::get<I>(tuple))...);
}

template<class ...tupleParams, typename makeIndexSequence = std::make_index_sequence<sizeof...(tupleParams)>>
auto flattenTuple(std::tuple<tupleParams...> tuple) {
    using K = typename NestedTypeGetter<std::tuple<tupleParams...>>::type_t;
    std::shared_ptr<Promise<K> > promisePtr(new Promise<K>);
    std::thread([promisePtr, &tuple]() {
        auto t = flatten(tuple, makeIndexSequence{});
        promisePtr->set(t);
    }).detach();
    return promisePtr->getFuture();
}