#include <iostream>
#include <cassert>
#include "reactive-framework-cpp.h"
using namespace std;
using namespace reacf;

int main(int argc, char **argv) {
  cout << "Started" << endl;
  function<float(int)> int2float = [](int i) -> float {
    return static_cast<float>(i);
  };

  Stream<int> s1("s1"); assert(s1.size() == 0);
  Stream<int> s2("s2"); assert(s2.size() == 0);
  Stream<int> &s = *join(s1, s2);
  assert(s1.size() == 1 && s2.size() == 1);
  assert(s.size() == 0);

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
  s2.set(e1);

  return 0;
}
