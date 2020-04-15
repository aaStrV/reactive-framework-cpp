/*
 * main.h
 *
 *  Created on: 5 мар. 2020 г.
 *      Author: Sergej_Pechenin
 *
 * Copyright (C) 2020 Sergej_Pechenin.
 *
 * License GPLv3+: GNU GPL version 3 or later
 * <https://gnu.org/licenses/gpl.html> This is free software: you are free to
 * change and redistribute it. There is NO WARRANTY, to the extent permitted by
 * law.
 */

#include <gtest.h>

#include <iostream>
//#include <vector>
#include "distributor_tests.h"
#include "property-tests.h"
#include "reactive-framework-cpp.h"
using namespace std;
using namespace reacf;

// TEST(GettimeofdayTest, base) {
//  struct timeval tv;
//  gettimeofday(&tv, NULL);
//  ASSERT_GT(tv.tv_sec, 1582744264);
//}

int main(int argc, char **argv) {
  //  cout << "Started" << endl;
  //  function<float(int)> int2float = [](int i) -> float {
  //    return static_cast<float>(i);
  //  };
  //
  //  Stream<int> s1;
  //  Stream<int> s2;
  //  Stream<int> &s = *join(s1, s2);
  //
  //  auto f1 = [](Event<int> &e) -> void {
  //    cout << e.getTimeStamp().tv_sec << "." << e.getTimeStamp().tv_usec << ":
  //    "; cout << "f1(): New event: value = " << e.getValue() << endl;
  //  };
  //  auto f2 = [](Event<int> &e) -> void {
  //    cout << e.getTimeStamp().tv_sec << "." << e.getTimeStamp().tv_usec << ":
  //    "; cout << "f2(): New event: value = " << e.getValue() << endl;
  //  };
  //  auto it_f1_0 = s.subscribe([](Event<int> &e) -> void {
  //    cout << e.getTimeStamp().tv_sec << "." << e.getTimeStamp().tv_usec << ":
  //    "; cout << "f1(): New event: value = " << e.getValue() << endl;
  //  });
  //  s.subscribe(f2);
  //
  //  s1.set(1);
  //  s2.set(2);
  //  s.unsubscribe(it_f1_0);
  //  Stream<float> &sf = *fmap(s, int2float);
  //  s1.set(3);
  //  Event<int> e1(4);
  //  auto it_f1_1 = s.subscribe(f1);
  //  s2.set(e1);

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
