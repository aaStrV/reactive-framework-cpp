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
  ASSERT_EQ(0, s1_test.size());
  int counter = 0;
  auto f1_test = [&counter](Event<int> &e) -> void {
    counter += 1;
  };
  auto it_f1 = s1_test.subscribe(f1_test);
  ASSERT_EQ(1, s1_test.size());
  s1_test.unsubscribe(it_f1);
  ASSERT_EQ(0, s1_test.size());
  auto f1_it_2 = s1_test.subscribe(f1_test);
  ASSERT_EQ(1, s1_test.size());
  ASSERT_EQ(0, counter);
  s1_test.push(1);
  ASSERT_EQ(1, counter);
  s1_test.push(2);
  ASSERT_EQ(2, counter);
  s1_test.push(3);
  ASSERT_EQ(3, counter);
  s1_test.unsubscribe(f1_it_2);
  auto f2_test = [&counter](const int &val) {
    counter = val;
  };
  auto f2_it = s1_test.subscribe(f2_test);
  s1_test.push(10);
  ASSERT_EQ(10, counter);
  s1_test.unsubscribe(f2_it);
  s1_test.push(11);
  ASSERT_EQ(10, counter);
  auto f2_it_2 = s1_test.subscribe(f2_test);
  s1_test.push(20);
  ASSERT_EQ(20, counter);
}

TEST(StreamTest, join) {
  Stream<int> s1_test;
  Stream<int> s2_test;
  ASSERT_EQ(0, s1_test.size());
  ASSERT_EQ(0, s2_test.size());
  Stream<int> &s_test = *join(s1_test, s2_test);
  ASSERT_EQ(0, s_test.size());
  ASSERT_EQ(1, s1_test.size());
  ASSERT_EQ(1, s2_test.size());
  int counter = 0;
  auto f1_test = [&counter](Event<int> &e) -> void {
    counter += 1;
  };
  auto it_f1 = s_test.subscribe(f1_test);
  ASSERT_EQ(1, s_test.size());
  s_test.unsubscribe(it_f1);
  ASSERT_EQ(0, s_test.size());
  s_test.subscribe(f1_test);
  ASSERT_EQ(1, s_test.size());
  ASSERT_EQ(0, counter);
  s1_test.push(1);
  ASSERT_EQ(1, counter);
  s2_test.push(2);
  ASSERT_EQ(2, counter);
  s_test.push(3);
  ASSERT_EQ(3, counter);
}

TEST(StreamTest, fmap) {
  function<float(int)> intDiv2 = [](int i) -> float {
    return static_cast<float>(i) / 2;
  };
  auto intDiv2_auto = [](int i) -> float {  // not working
    return static_cast<float>(i) / 2;
  };
  struct intDiv2_f : function<float(int)> {
    float operator()(int i) {
      return static_cast<float>(i) / 2;
    }
  };
  float foo = 0;
  auto test = [&foo](Event<float> &e) -> void {
    foo = e.getValue();
  };
  Stream<int> s_int("s_int");
  s_int.push(1);
  Stream<float> &s_float = *fmap(s_int, intDiv2);
  s_float.subscribe(test);
  ASSERT_FLOAT_EQ(0.0, foo);
  s_int.push(1);
  ASSERT_FLOAT_EQ(0.5, foo);
  s_int.push(2001);
  ASSERT_FLOAT_EQ(1000.5, foo);
  s_int.push(3);
  ASSERT_FLOAT_EQ(1.5, foo);
}

TEST(StreamTest, filter) {
  function<bool(int)> filterF = [](int i) -> bool {
    return i % 2 == 0 ? true : false;
  };
  Stream<int> s1;
  Stream<int> &s2 = *filter(s1, filterF);

  int counter = 0;
  function<void(const int val)> testF = [&counter](int i){
    counter = i;
  };
  s2.subscribe(testF);

  ASSERT_EQ(0, counter);
  s1.push(1);
  ASSERT_EQ(0, counter);
  s1.push(2);
  ASSERT_EQ(2, counter);
  s1.push(3);
  ASSERT_EQ(2, counter);
  s1.push(1001);
  ASSERT_EQ(2, counter);
  s1.push(11000);
  ASSERT_EQ(11000, counter);

}

int main(int argc, char **argv) {
  cout << "Started" << endl;
  function<float(int)> int2float = [](int i) -> float {
    return static_cast<float>(i);
  };

  Stream<int> s1("s1");
  Stream<int> s2("s2");
  Stream<int> &s = *join(s1, s2);

  auto f1 = [](Event<int> &e) -> void {
    cout << e.getTimeStamp().tv_sec << "." << e.getTimeStamp().tv_usec << ": ";
    cout << "f1(): New event: value = " << e.getValue() << endl;
  };
  auto f2 = [](Event<int> &e) -> void {
    cout << e.getTimeStamp().tv_sec << "." << e.getTimeStamp().tv_usec << ": ";
    cout << "f2(): New event: value = " << e.getValue() << endl;
  };
  auto it_f1_0 = s.subscribe([](Event<int> &e) -> void {
    cout << e.getTimeStamp().tv_sec << "." << e.getTimeStamp().tv_usec << ": ";
    cout << "f1(): New event: value = " << e.getValue() << endl;
  });
  s.subscribe(f2);

  s1.push(1);
  s2.push(2);
  s.unsubscribe(it_f1_0);
  Stream<float> &sf = *fmap(s, int2float);
  s1.push(3);
  Event<int> e1(4);
  auto it_f1_1 = s.subscribe(f1);
  s2.push(e1);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
