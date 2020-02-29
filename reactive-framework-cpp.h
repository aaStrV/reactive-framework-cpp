#ifndef REACTIVE_FRAMEWORK_CPP_H_
#define REACTIVE_FRAMEWORK_CPP_H_
/**
 * Target
 *    Get a system of reactive objects.
 *
 *    Assume something like this:
 *      reacf::Property a(0), b(0), result(0), c(0);
 *      result = a + b; // result == 0;
 *      a = 10; // result == 10;
 *      c = a; // c == 10
 *      b = 20; // result == 30, c == 30;
 *
 * Requirements
 *    - System should support distributed applications development;
 */
//#define DEBUG
#include <list>
#include <map>
#include <set>
#include <functional>
#include <string>
#include "_gettimeofday.h"
#include "Event.h"
#include "Dispetcher.h"
#include "Stream.h"

namespace reacf {
/**
 * 'join' make a new stream type A from 2 type A streams
 */
template<typename T>
Stream<T>* join(Stream<T> &s1, Stream<T> &s2) {
  Stream<T> *result = new Stream<T>;  // don't delete this!

  auto connectStream = [result](Event<T> &e) -> void {
    (*result).push(e);
  };

  typename Stream<T>::it_t it1 = s1.subscribe(connectStream);
  typename Stream<T>::it_t it2 = s2.subscribe(connectStream);
  result->sources_.insert(&s1);
  s1.targets_.insert(std::make_pair(result, it1));
  result->sources_.insert(&s2);
  s2.targets_.insert(std::make_pair(result, it2));

  return result;
}

/**
 * 'fmap' reflects source stream, type A, on a new stream, type B.
 * E.g. Stream<int> -> Stream<bool>. Needs function that convert type A element
 * to type B element
 */
template<typename T_source, typename T_dest>
Stream<T_dest>* fmap(Stream<T_source> &s, std::function<T_dest(T_source)> f) {
  Stream<T_dest> *result = new Stream<T_dest>;  // don't delete this!

  auto convertEvent = [result, f](Event<T_source> &e) -> void {
    (*result).push(f(e.getValue()));
  };

  s.subscribe(convertEvent);
//  typename Stream<T_source>::it_t it = s.subscribe(convertEvent);
//  result->sources_.insert(std::make_pair(&s, it));

  return result;
}

/**
 * 'filter'
 */
template<typename T>
Stream<T>* filter(Stream<T> &s, std::function<bool(const T&)> f) {
  Stream<T> *result = new Stream<T>;  // don't delete this!

  auto filterEvent = [result, f](Event<T> &e) -> void {
    if (f(e.getValue()) == true) {
      (*result).push(e.getValue());
    }
  };
  s.subscribe(filterEvent);
  return result;
}

/**
 * 'fold'
 */
template<typename T_stream, typename T_acc>
void fold(Stream<T_stream> &s, std::function<void(const T_stream&, T_acc&)> f,
          T_acc &acc) {

  auto foldF = [&acc, f](Event<T_stream> &e) {
    f(e.getValue(), acc);
  };
  s.subscribe(foldF);
}
}  // namespace reacf
#endif /* REACTIVE_FRAMEWORK_CPP_H_ */
