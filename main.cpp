#include <iostream>
#include <gtest.h>
#include "reactive-framework-cpp.h"
using namespace std;
using namespace reacf;

TEST(gettimeofdayTest, base) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  cout << "tv.tv_sec = " << tv.tv_sec << endl;
  cout << "tv.tv_usec = " << tv.tv_usec << endl;
  ASSERT_GT(tv.tv_sec, 1582744264);
}

TEST(StreamTest, base) {
  Stream<int> s;
  auto f1 = [](Event<int> &e) -> void {
    cout << "f1(): New event: value = " << e.getValue() << endl;
  };
  auto f2 = [](Event<int> &e) -> void {
    cout << "f2(): New event: value = " << e.getValue() << endl;
  };
  s.subscribe(f1);
  s.subscribe(f2);
  s.push(1);
  s.push(2);
  s.unsubscribe(f1);
  s.push(3);
  Event<int> e1(4);
  s.push(e1);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
