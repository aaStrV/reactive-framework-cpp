/*
 * stream_tests.h
 *
 *  Created on: 29 февр. 2020 г.
 *      Author: sergius
 */

#ifndef STREAM_TESTS_H_
#define STREAM_TESTS_H_
#include <vector>
#include "reactive-framework-cpp.h"

TEST(StreamTest, general) {
  using namespace reacf;
  Stream<int> s1_test;
  ASSERT_EQ(0, s1_test.observersSize());
  int counter = 0;
  Stream<int>::event_fun_t f1_test = [&counter](Event<int> &e) -> void {
    counter += 1;
  };
  Observable* it_f1 = s1_test.subscribe(f1_test);
  ASSERT_EQ(1, s1_test.observersSize());
  s1_test.unsubscribe(it_f1);
  ASSERT_EQ(0, s1_test.observersSize());
  auto f1_it_2 = s1_test.subscribe(f1_test);
  ASSERT_EQ(1, s1_test.observersSize());
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

TEST(StreamTest, joinMethod) {
  using namespace reacf;

  Stream<int> s1;
  ASSERT_EQ(0, s1.observersSize());
  ASSERT_EQ(0, s1.observablesSize());

  Stream<int> s2;
  ASSERT_EQ(0, s2.observersSize());
  ASSERT_EQ(0, s2.observablesSize());

  Stream<int> *p_s3 = s1.join(s2);
  ASSERT_EQ(1, s1.observersSize());
  ASSERT_EQ(0, s1.observablesSize());
  ASSERT_EQ(1, s2.observersSize());
  ASSERT_EQ(0, s2.observablesSize());
  ASSERT_EQ(0, p_s3->observersSize());
  ASSERT_EQ(2, p_s3->observablesSize());

  Stream<int>::fun_t printF = [](int a) {
//    std::cout << "TEST(StreamTest, joinMethod): event value = "<<a << std::endl;
  };

  int counter = 0;
  Stream<int>::fun_t testF = [&counter](int a) {
    counter = a;
  };

  auto p_printF = p_s3->subscribe(printF);
  ASSERT_EQ(1, s1.observersSize());
  ASSERT_EQ(0, s1.observablesSize());
  ASSERT_EQ(1, s2.observersSize());
  ASSERT_EQ(0, s2.observablesSize());
  ASSERT_EQ(1, p_s3->observersSize());
  ASSERT_EQ(2, p_s3->observablesSize());

  auto p_testF = p_s3->subscribe(testF);
  ASSERT_EQ(1, s1.observersSize());
  ASSERT_EQ(0, s1.observablesSize());
  ASSERT_EQ(1, s2.observersSize());
  ASSERT_EQ(0, s2.observablesSize());
  ASSERT_EQ(2, p_s3->observersSize());
  ASSERT_EQ(2, p_s3->observablesSize());

  ASSERT_EQ(0, counter);
  s1.push(121);
  ASSERT_EQ(121, counter);
  p_s3->unsubscribe(p_testF);
  ASSERT_EQ(1, s1.observersSize());
  ASSERT_EQ(0, s1.observablesSize());
  ASSERT_EQ(1, s2.observersSize());
  ASSERT_EQ(0, s2.observablesSize());
  ASSERT_EQ(1, p_s3->observersSize());
  ASSERT_EQ(2, p_s3->observablesSize());

  s1.push(2);
  ASSERT_EQ(121, counter);
  p_testF = p_s3->subscribe(testF);
  ASSERT_EQ(1, s1.observersSize());
  ASSERT_EQ(0, s1.observablesSize());
  ASSERT_EQ(1, s2.observersSize());
  ASSERT_EQ(0, s2.observablesSize());
  ASSERT_EQ(2, p_s3->observersSize());
  ASSERT_EQ(2, p_s3->observablesSize());

  s1.push(333);
  ASSERT_EQ(333, counter);
}

TEST(StreamTest, join) {
  using namespace reacf;
  using std::cout;
  using std::endl;
  using std::vector;

  Stream<float> s1, s2, s3;
  ASSERT_EQ(0, s1.observersSize());
  ASSERT_EQ(0, s2.observersSize());
  ASSERT_EQ(0, s3.observersSize());

  vector<Stream<float>*> v {&s1, &s2, &s3};
  Stream<float> *p_s4 = _very_bad_join(v);
  ASSERT_EQ(1, s1.observersSize());
  ASSERT_EQ(1, s2.observersSize());
  ASSERT_EQ(1, s3.observersSize());
  ASSERT_EQ(0, p_s4->observersSize());
  ASSERT_EQ(2, p_s4->observablesSize());

  Stream<float> *p_s5 = join(s1, s2);
  ASSERT_EQ(2, s1.observersSize());
  ASSERT_EQ(2, s2.observersSize());
  ASSERT_EQ(1, s3.observersSize());
  ASSERT_EQ(0, p_s4->observersSize());
  ASSERT_EQ(2, p_s4->observablesSize());
  ASSERT_EQ(0, p_s4->observersSize());
  ASSERT_EQ(2, p_s5->observablesSize());
}

TEST(StreamTest, createAndDestruct) {
  using namespace reacf;
  // TODO: many times test for memory leaks seaching
}

TEST(StreamTest, filter) {
  using namespace reacf;

  Stream<int> s1;
  std::function<bool(int)> filterEvenF = [](int a) {
    return a%2==0? true: false;
  };
  Stream<int> *p_s2 = filter(s1, filterEvenF);

  int icount = 0;
  std::function<void(int)> testF = [&icount](int a) {
    icount = a;
  };
  p_s2->subscribe(testF);

  s1.push(1);
  ASSERT_EQ(0, icount);
  s1.push(2);
  ASSERT_EQ(2, icount);
  s1.push(3);
  ASSERT_EQ(2, icount);
  s1.push(4);
  ASSERT_EQ(4, icount);

//  std::function<bool(Event<int> &)> filterEvenMicrosecF = [](Event<int> & e) {
//    return e.getTimeStamp().tv_usec%2==0? true: false;
//  };
//  Dispetcher<Event<int>&> *p_d3 = filter(s1, filterEvenMicrosecF);
//  Stream<int> *p_s3 = p_d3;
//  Stream<int> *p_s3 = (Stream<int> *)filter(s1, filterEvenMicrosecF);
//  p_s3->push(22);
}

#endif /* STREAM_TESTS_H_ */
