/*
 * main.h
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

#include <iostream>
#include <cassert>
#include "reactive-framework-cpp.h"
using namespace std;
using namespace reacf;

// @formatter:off
int main(int argc, char **argv)
{
    cout << "Started" << endl;

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


       assert( 0 == Arrow::count );
       assert( 0 == Object::count );
   }

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
       assert( abs(A.getValue() - 0) <= 0.001 );
       assert( abs(B.getValue() - 0) <= 0.001 );
       assert( abs(C.getValue() - 0) <= 0.001 );
       assert( abs(D.getValue() - 0) <= 0.001 );
       assert( abs(E.getValue() - 0) <= 0.001 );
       assert( F.getValue() != F.getValue() ); // Nan != NaN
       assert( abs(G.getValue() - 0.0001) <= 0.001 );

       A = 10;
       B = 12;
       assert( abs(A.getValue() - 10) <= 0.001 );
       assert( abs(B.getValue() - 12) <= 0.001 );
       assert( abs(C.getValue() - 22) <= 0.001 );
       assert( abs(D.getValue() - (-2)) <= 0.001 );
       assert( abs(E.getValue() - 120) <= 0.001 );
       assert( abs(F.getValue() - 10.0/12) <= 0.001 );

       I = 111;
       J = 3;
       assert( H.getValue() == 113%3 );
   }

   #endif /* PROPERTY_TESTS_H_ */

    cout << "Finished" << endl;
    return 0;
}
// @formatter:on
