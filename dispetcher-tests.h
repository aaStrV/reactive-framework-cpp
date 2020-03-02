#ifndef DISPETCHER_TESTS_H_
#define DISPETCHER_TESTS_H_
#include "reactive-framework-cpp.h"

TEST(DispetcherTest, fmapMethod) {
  using namespace std;
  using namespace reacf;

  function<float(int)> int2floatF = [](int a) {
    return static_cast<float>(a);
  };

  Dispetcher<int> d;
//  ASSERT_EQ(0, d.observablesSize());
//  ASSERT_EQ(0, d.observersSize());
  Dispetcher<float> *df = d.fmap(int2floatF);
//  ASSERT_EQ(1, df->observablesSize());
//  ASSERT_EQ(0, df->observersSize());
//  ASSERT_EQ(0, d.observablesSize());
//  ASSERT_EQ(1, d.observersSize());

  float fcounter = 0;
  function<void(float)> testFloatF = [&fcounter](float a) {
    fcounter = a;
  };

  int icounter = 0;
  function<void(int)> testIntF = [&icounter](int a) {
    icounter = a;
  };
  d.subscribe(testIntF);
//  ASSERT_EQ(0, d.observablesSize());
//  ASSERT_EQ(2, d.observersSize());
//  ASSERT_EQ(1, df->observablesSize());
//  ASSERT_EQ(0, df->observersSize());

  auto p_printF = df->subscribe(testFloatF);

  ASSERT_FLOAT_EQ(0, fcounter);
  ASSERT_EQ(0, icounter);
  d.publish(11);
  ASSERT_FLOAT_EQ(11, fcounter);
  ASSERT_EQ(11, icounter);
  d.publish(200);
  ASSERT_FLOAT_EQ(200, fcounter);
  ASSERT_EQ(200, icounter);
  df->unsubscribe(p_printF);
  d.publish(227);
  ASSERT_FLOAT_EQ(200, fcounter);
  ASSERT_EQ(227, icounter);
  p_printF = df->subscribe(testFloatF);
  d.publish(30654);
  ASSERT_FLOAT_EQ(30654, fcounter);
  ASSERT_EQ(30654, icounter);

  function<double(float)> floatSquareF = [](float a)->double {
    return static_cast<double>(a*a);
  };

  Dispetcher<double> *dd = fmap(*df, floatSquareF);
//  ASSERT_EQ(0, d.observablesSize());
//  ASSERT_EQ(2, d.observersSize());
//  ASSERT_EQ(1, df->observablesSize());
//  ASSERT_EQ(2, df->observersSize());
//  ASSERT_EQ(1, dd->observablesSize());
//  ASSERT_EQ(0, dd->observersSize());

  double dcounter = 0;
  function<void(double)> testDoubleF = [&dcounter](double a) {
    dcounter = a;
  };
  auto p_testFloatF = dd->subscribe(testDoubleF);
//  ASSERT_EQ(0, d.observablesSize());
//  ASSERT_EQ(2, d.observersSize());
//  ASSERT_EQ(1, df->observablesSize());
//  ASSERT_EQ(2, df->observersSize());
//  ASSERT_EQ(1, dd->observablesSize());
//  ASSERT_EQ(1, dd->observersSize());

  d.publish(11);
//  ASSERT_DOUBLE_EQ(121, dcounter);
//  ASSERT_EQ(11, icounter);

  delete df;
//  ASSERT_EQ(0, d.observablesSize());
//  ASSERT_EQ(1, d.observersSize());
//  ASSERT_EQ(1, df->observablesSize());
//  ASSERT_EQ(2, df->observersSize());
//  ASSERT_EQ(0, dd->observablesSize());
//  ASSERT_EQ(1, dd->observersSize());

  d.publish(1131);
//  ASSERT_DOUBLE_EQ(121, dcounter);
//  ASSERT_EQ(1131, icounter);
}

TEST(DispetcherTest, filter) {
  using namespace reacf;

  Dispetcher<int> d1;
  std::function<bool(int)> filterEvenF = [](int a) {
    return a%2==0? true: false;
  };
  Dispetcher<int> *p_d2 = filter(d1, filterEvenF);

  int icount = 0;
  std::function<void(int)> testF = [&icount](int a){
    icount = a;
  };
  p_d2->subscribe(testF);

  d1.publish(1);
  ASSERT_EQ(0, icount);
  d1.publish(2);
  ASSERT_EQ(2, icount);
  d1.publish(3);
  ASSERT_EQ(2, icount);
  d1.publish(4);
  ASSERT_EQ(4, icount);
}

#endif /* DISPETCHER_TESTS_H_ */
