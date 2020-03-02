#ifndef OBSERVABLE_TESTS_H_
#define OBSERVABLE_TESTS_H_
#include "reactive-framework-cpp.h"

TEST(ObservableTest, general){
  using namespace reacf;

  int counter = 0;
  Dispetcher<int> &d1 = *(new Dispetcher<int>);

  auto printF = [&counter](int a){
    counter = a;
  };
  ASSERT_EQ(0, d1.observersSize());
  ASSERT_EQ(0, d1.observablesSize());
  Observable * p_printF = d1.subscribe(printF);

  ASSERT_EQ(1, d1.observersSize());
  ASSERT_EQ(0, d1.observablesSize());

  int a = 10;
  d1.publishReference(a);
  ASSERT_EQ(10, counter);
  d1.publish(20);
  ASSERT_EQ(20, counter);

  d1.unsubscribe(p_printF);
  d1.publish(3);
  ASSERT_EQ(20, counter);

  delete(&d1);
  ASSERT_EQ(0, d1.observersSize());
}

TEST(ObserverTest, general){
  using namespace reacf;

  int counter = 0;

  std::function<void(int&)> printF = [&counter](int &a){
    counter = a;
  };

  Observer<void, int&> o(printF);
  int a=10;
  o(a);
  ASSERT_EQ(10, counter);

  std::function<void(int)> printF1 = [&counter](int a){
    counter = a;
  };

  Observer<void, int> o1(printF1);
  o1(20);
  ASSERT_EQ(20, counter);
}


#endif /* OBSERVABLE_TESTS_H_ */
