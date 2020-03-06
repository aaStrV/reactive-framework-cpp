/*
 * property-tests.h
 *
 *  Created on: 5 мар. 2020 г.
 *      Author: Sergej_Pechenin
 *
 * Copyright (C) 2020 Sergej_Pechenin.
 *
 * License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */

#ifndef PROPERTY_TESTS_H_
#define PROPERTY_TESTS_H_
#include "reactive-framework-cpp.h"

TEST(PropertyTest, creating)
{
    using namespace reacf;
    using std::cout;
    using std::endl;

    Property<int> *a = new Property<int>(1); // A(0); // 2 obj
    Property<int> *b = new Property<int>(2);// B(0); B(0);// 4 obj
    Property<int> *c = new Property<int>(3);// C(0); C(0);// 6 obj

    *a = *b - *c;// C = A+B;// 12 obj, 8 Arrows, +6 Objects, +5 Arrows
    // 9 obj -3 Objects, -? Arrows

    delete a;// A_O_T:6_8_6
    delete b;// A_O_T:4_6_4
    delete c;// A_O_T:2_3_2

    ASSERT_EQ(0, Arrow::count);
    ASSERT_EQ(0, Object::count);
}

TEST(PropertyTest, plus)
{
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
    assert( abs(A.getValue() - 0.0001) <= 0.001 );
    ASSERT_FLOAT_EQ(0, A.getValue());
    ASSERT_FLOAT_EQ(0, B.getValue());
    ASSERT_FLOAT_EQ(0, C.getValue());
    ASSERT_FLOAT_EQ(0, D.getValue());
    ASSERT_FLOAT_EQ(0, E.getValue());
    ASSERT_NE(F.getValue(), F.getValue()); // Nan != NaN
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
