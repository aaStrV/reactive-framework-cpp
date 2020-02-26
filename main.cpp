#include <iostream>
#include <gtest.h>
#include "reactive-framework-cpp.h"
using namespace std;

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
