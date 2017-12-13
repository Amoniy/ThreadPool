cmake_minimum_required(VERSION 3.8)
project(cpphometasks)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(lib/googletest-master)
include_directories(lib/googletest-master/googletest/include)
include_directories(lib/googletest-master/googlemock/include)

add_definitions(-D_GTEST)
add_definitions(-D_GLIBCXX_DEBUG)

file(GLOB_RECURSE SOURCE_FILES *.cpp tests/*.cpp)
file(GLOB_RECURSE HEADER_FILES *.h tests/*.h)

include_directories(include)

if ("$ENV{GTEST}" STREQUAL "y")
    message("!Gtests're enabled!")

    find_package(GTest REQUIRED)

    include_directories(tests)
    add_definitions(-D_GTEST)
endif ()

set(SOURCE_FILES main.cpp Map.h ThreadPool.h Promise.h Future.h SharedState.h FlattenTuple.h tests/map_test.cpp tests/promise_test.cpp tests/flatten_test.cpp Flatten.h ThreadPool.cpp)
add_executable(cpphometasks ${SOURCE_FILES})

if ("$ENV{GTEST}" STREQUAL "y")
    target_link_libraries(${PROJECT_NAME} ${GTEST_BOTH_LIBRARIES} gmock)
endif ()

target_link_libraries(${PROJECT_NAME} gtest gtest_main)
target_link_libraries(${PROJECT_NAME} pthread)