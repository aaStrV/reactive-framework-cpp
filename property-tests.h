#ifndef PROPERTY_TESTS_H_
#define PROPERTY_TESTS_H_
#include "reactive-framework-cpp.h"

TEST(PropertyTest, plus){
  using namespace reacf;
  using std::cout;
  using std::endl;

  Property<float> A(0), B(0), C(0), D(0), E(0), F(0), G(0);
  Property<int> H(0), I(0), J(1);
  C = A + B;
  D = A - B;
  E = A * B;
  F = A / B;
  H = I % J;
  ASSERT_FLOAT_EQ(0, A.getValue());
  ASSERT_FLOAT_EQ(0, B.getValue());
  ASSERT_FLOAT_EQ(0, C.getValue());
  ASSERT_FLOAT_EQ(0, D.getValue());
  ASSERT_FLOAT_EQ(0, E.getValue());
  ASSERT_FLOAT_EQ(0, F.getValue());
  ASSERT_FLOAT_EQ(0, G.getValue());

  A = 10;
  B = 12;
  ASSERT_FLOAT_EQ(10, A.getValue());
  ASSERT_FLOAT_EQ(12, B.getValue());
  ASSERT_FLOAT_EQ(22, C.getValue());
  ASSERT_FLOAT_EQ(-2, D.getValue());
  ASSERT_FLOAT_EQ(120, E.getValue());
  ASSERT_FLOAT_EQ(10.0/12, F.getValue());

  I = 111;
  J = 3;
  ASSERT_FLOAT_EQ(111%3, H.getValue());
}


#endif /* PROPERTY_TESTS_H_ */
