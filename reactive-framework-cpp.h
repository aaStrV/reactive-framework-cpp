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
#include <vector>
#include <functional>
#include <string>

#include "defines.h"
#include "_gettimeofday.h"
#include "Event.h"
#include "Stream.h"
#include "Observable.h"
#include "Dispetcher.h"
#include "Distributor.h"

namespace reacf {
/**
 * 'join' make a new stream type A from 2 type A streams
 */
template<typename T>
___Stream <T>* ___join(___Stream <T> &s1, ___Stream <T> &s2) {
  ___Stream <T> *result = new ___Stream <T>;  // don't delete this!

  auto connectStream = [result](Event<T> &e) -> void {
    (*result).push(e);
  };

  typename ___Stream<T>::it_t it1 = s1.subscribe(connectStream);
  typename ___Stream<T>::it_t it2 = s2.subscribe(connectStream);
  result->sources_.insert(&s1);
  s1.targets_.insert(std::make_pair(result, it1));
  result->sources_.insert(&s2);
  s2.targets_.insert(std::make_pair(result, it2));

  return result;
}

/**
 * TODO: we create unnamed streams here.
 * If we remove some stream in future, we
 * will have memory leak. This is bad function.
 * Very bad
 */
template<typename T>
Stream<T>* _very_bad_join(std::vector<Stream<T>*> v) {
  if (v.size() == 0)
    return nullptr;

  Stream<T> *new_stream = nullptr;

  auto it = v.begin(), end = v.end();
  new_stream = *it;  // pointer to first

  if (v.size() == 1)
    return new_stream->join(*new_stream);
  it += 1;
  for (; it != end; ++it) {
    DPRINT("Joining stream ");
    DPRINTLN(*it);
    new_stream = new_stream->join(*(*it));
    DPRINT("New stream: "); DPRINTLN(new_stream);
  }
  return new_stream;
}

template<typename T>
Stream<T>* join(Stream<T> &s1, Stream<T> &s2) {
  return s1.join(s2);
}

/**
 * 'fmap' reflects source stream, type A, on a new stream, type B.
 * E.g. Stream<int> -> Stream<bool>. Needs function that convert type A element
 * to type B element
 */
template<typename T_source, typename T_dest>
___Stream <T_dest>* ___fmap(___Stream <T_source> &s,
                            std::function<T_dest(T_source)> f) {
  ___Stream <T_dest> *result = new ___Stream <T_dest>;  // don't delete this!

  auto convertEvent = [result, f](Event<T_source> &e) -> void {
    (*result).push(f(e.getValue()));
  };

  s.subscribe(convertEvent);
//  typename Stream<T_source>::it_t it = s.subscribe(convertEvent);
//  result->sources_.insert(std::make_pair(&s, it));

  return result;
}

template<typename Dest, typename Source>
Dispetcher<Dest>* fmap(Dispetcher<Source> &d, std::function<Dest(Source)> f) {
  return d.fmap(f);
}

//template<typename T_source, typename T_dest>
//Dispetcher<T_dest>* fmap(Dispetcher<T_source> &s,
//                         std::function<T_dest(T_source&)> f) {
//  return s.fmap(f);
//}

/**
 * 'filter'
 */
template<typename T>
___Stream <T>* ___filter(___Stream <T> &s, std::function<bool(const T&)> f) {
  ___Stream <T> *result = new ___Stream <T>;  // don't delete this!

  auto filterEvent = [result, f](Event<T> &e) -> void {
    if (f(e.getValue()) == true) {
      (*result).push(e.getValue());
    }
  };
  s.subscribe(filterEvent);
  return result;
}

template<typename T>
Dispetcher<T> *filter(Dispetcher<T> &d, std::function<bool(T)> f){
  DPRINTLN("filter<T>()");
  return d.filter(f);
}

template<typename T>
Stream<T> *filter(Stream<T> &s, std::function<bool(T)> f){
  DPRINTLN("filter<Event<T>&>()");
  return s.filter(f);
}

/**
 * 'fold'
 */
template<typename T_stream, typename T_acc>
void fold(___Stream <T_stream> &s,
          std::function<void(const T_stream&, T_acc&)> f, T_acc &acc) {

  auto foldF = [&acc, f](Event<T_stream> &e) {
    f(e.getValue(), acc);
  };
  s.subscribe(foldF);
}
}  // namespace reacf
#endif /* REACTIVE_FRAMEWORK_CPP_H_ */
