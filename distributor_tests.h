/*
 * distributor-tests.h
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

#ifndef DISTRIBUTOR_TESTS_H_
#define DISTRIBUTOR_TESTS_H_
#include "reactive-framework-cpp.h"

TEST(DistributorTest, general) {
  using namespace reacf;
  using std::cout;
  using std::endl;

  Distributor<int> *d = new Distributor<int>;

  int icount = 0;
  Distributor<int>::fun_t printIntF = [&icount](int a) { icount = a; };
  auto o = d->subscribe(printIntF);
  d->publish(1);
  ASSERT_EQ(1, icount);
  d->unsubscribe(o);
  d->publish(321);
  ASSERT_EQ(1, icount);
  o = d->subscribe(printIntF);
  d->publish(7767);
  ASSERT_EQ(7767, icount);
  d->publish(165);
  ASSERT_EQ(165, icount);

  delete d;
  ASSERT_EQ(0, Arrow::count);
  ASSERT_EQ(0, Object::count);
}

TEST(DistributorTest, fmap) {
  using namespace reacf;
  using std::cout;
  using std::endl;

  int fcount = 0;
  std::function<float(int)> int2floatF = [](int a) {
    return static_cast<float>(a);
  };

  Distributor<int> *d = new Distributor<int>;
  Distributor<float> *df = d->fmap(int2floatF);

  std::function<void(float)> testFloatF = [&fcount](float a) { fcount = a; };
  auto p = df->subscribe(testFloatF);

  ASSERT_FLOAT_EQ(0, fcount);
  d->publish(100);
  ASSERT_FLOAT_EQ(100, fcount);
  df->unsubscribe(p);
  d->publish(532);
  ASSERT_FLOAT_EQ(100, fcount);
  p = df->subscribe(testFloatF);
  d->publish(324);
  ASSERT_FLOAT_EQ(324, fcount);
  d->publish(5322);
  ASSERT_FLOAT_EQ(5322, fcount);

  delete d;
  delete df;
  ASSERT_EQ(0, Arrow::count);
  ASSERT_EQ(0, Object::count);
}

TEST(DistributorTest, filter) {
  using namespace reacf;

  std::function<bool(int)> filterEvenF = [](int a) {
    return a % 2 == 0 ? true : false;
  };
  Distributor<int> *d1 = new Distributor<int>;
  Distributor<int> *d2 = d1->filter(filterEvenF, 255);

  int icount = 0;
  std::function<void(int)> testF = [&icount](int a) { icount = a; };
  d2->subscribe(testF);

  ASSERT_EQ(0, icount);
  d1->publish(1);
  ASSERT_EQ(255, icount);
  d1->publish(2);
  ASSERT_EQ(2, icount);
  d1->publish(3);
  ASSERT_EQ(255, icount);
  d1->publish(4);
  ASSERT_EQ(4, icount);

  delete d2;
  delete d1;
  ASSERT_EQ(0, Arrow::count);
  ASSERT_EQ(0, Object::count);
}

#endif /* DISTRIBUTOR_TESTS_H_ */
