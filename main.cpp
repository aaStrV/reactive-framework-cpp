#include <iostream>
#include <gtest.h>
#include <sys/time.h>
#include "reactive-framework-cpp.h"
using namespace std;

TEST(gettimeofdayTest, base) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  cout << "tv.tv_sec = " << tv.tv_sec << endl;
  cout << "tv.tv_usec = " << tv.tv_usec << endl;
  ASSERT_GT(tv.tv_sec, 1582744264);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
