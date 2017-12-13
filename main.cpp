#ifdef _GTEST

#include <gtest/gtest.h>

#endif

using namespace std;

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}