#ifndef DISTRIBUTOR_TESTS_H_
#define DISTRIBUTOR_TESTS_H_
#include "reactive-framework-cpp.h"

TEST(DistributorTest, general) {
  using namespace reacf;
  using std::cout;
  using std::endl;

  Distributor<int> *d = new Distributor<int>;

  int icount = 0;
  Distributor<int>::fun_t printIntF = [&icount](int a) {
    DPRINT("Distributed value: ");
    DPRINTLN(a);
    icount = a;
  };
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
}

#endif /* DISTRIBUTOR_TESTS_H_ */
