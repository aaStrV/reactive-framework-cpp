#include <iostream>
#include <gtest.h>
#include "reactive-framework-cpp.h"
using namespace std;
using namespace reacf;

TEST(gettimeofdayTest, base) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  ASSERT_GT(tv.tv_sec, 1582744264);
}

TEST(StreamTest, base) {
  Stream<int> s1_test;
  Stream<int> s2_test;
  ASSERT_EQ(0, s1_test.size());
  ASSERT_EQ(0, s2_test.size());
  Stream<int> &s_test = join(s1_test, s2_test);
  ASSERT_EQ(0, s_test.size());
  ASSERT_EQ(1, s1_test.size());
  ASSERT_EQ(1, s2_test.size());
  int counter = 0;
  cout << "1) counter = " << counter << endl;
  auto f1_test = [&counter](Event<int> &e) -> void {
    counter += 1;
  };
  auto it_f1 = s_test.subscribe(f1_test);
  ASSERT_EQ(1, s_test.size());
  s_test.unsubscribe(it_f1);
  ASSERT_EQ(0, s_test.size());
  s_test.subscribe(f1_test);
  ASSERT_EQ(0, counter);
  s1_test.push(1);
  ASSERT_EQ(1, counter);
  s2_test.push(2);
  ASSERT_EQ(2, counter);
  s_test.push(3);
  ASSERT_EQ(3, counter);
}

int main(int argc, char **argv) {
  cout << "Started" << endl;
  Stream<int> s1;
  Stream<int> s2;
  Stream<int> &s = join(s1, s2);
  auto f1 = [](Event<int> &e) -> void {
    cout << e.getTimeStamp().tv_sec << "." << e.getTimeStamp().tv_usec << ": ";
    cout << "f1(): New event: value = " << e.getValue() << endl;
  };
  auto f2 = [](Event<int> &e) -> void {
    cout << e.getTimeStamp().tv_sec << "." << e.getTimeStamp().tv_usec << ": ";
    cout << "f2(): New event: value = " << e.getValue() << endl;
  };
  auto it_f1_0 = s.subscribe(f1);
  s.subscribe(f2);
  s1.push(1);
  s2.push(2);
  s.unsubscribe(it_f1_0);
  s1.push(3);
  Event<int> e1(4);
  auto it_f1_1 = s.subscribe(f1);
  s2.push(e1);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
