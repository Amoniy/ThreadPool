#include <gtest/gtest.h>
#include "Promise.h"
#include "FlattenTuple.h"
#include <ostream>

using namespace std;

template<class Ch, class Tr, typename T>
decltype(auto) operator<<(std::basic_ostream<Ch, Tr> &os,
                          const vector<T> &t) {
    os << "[ ";
    for (auto &i: t) {

        os << i;
        if (i != t.back())
            os << ", ";
    }
    return os << "]";
}

template<class Ch, class Tr, typename T, typename L>
decltype(auto) operator<<(std::basic_ostream<Ch, Tr> &os,
                          const map<T, L> &t) {
    os << "[ ";
    for (auto &i: t) {
        os << "{ " << i.first << ": " << i.second << "}, ";

    }
    return os << "]";
}

template<class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple_impl(std::basic_ostream<Ch, Tr> &os,
                      const Tuple &t,
                      std::index_sequence<Is...>) {
    ((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

template<class Ch, class Tr, class... Args>
decltype(auto) operator<<(std::basic_ostream<Ch, Tr> &os,
                          const std::tuple<Args...> &t) {
    os << "(";
    print_tuple_impl(os, t, std::index_sequence_for<Args...>{});
    return os << ")";
}

int main(int argc, char *argv[]) {
    int integer;
    Promise<int> promiseInt;
    Promise<std::vector<int>> promiseVector;
    Promise<std::map<std::string, int>> promiseMap;
    Promise<Future<std::vector<int>>> promiseFuture;
    auto t = make_tuple(integer, promiseInt.getFuture(), promiseFuture.getFuture(), promiseMap.getFuture());
    vector<int> k = {1, 2, 3, 4, 5};
    map<std::string, int> k2 = {{"first",  11},
                                {"second", 22},
                                {"third",  33}};
    integer = 0;
    promiseInt.set(1);
    promiseVector.set(k);
    promiseMap.set(k2);
    promiseFuture.set(std::move(promiseVector.getFuture()));
    auto s = flattenTuple(move(t));
    cout << s.get() << endl;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}