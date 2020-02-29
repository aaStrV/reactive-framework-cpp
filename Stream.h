/*
 * Stream.h
 *
 *  Created on: 29 февр. 2020 г.
 *      Author: sergius
 */

#ifndef STREAM_H_
#define STREAM_H_
#include "Event.h"
#include "Dispetcher.h"

namespace reacf {
/**
 * 'Stream' is an 'Event' dispetcher
 *
 * Methods:
 *  subscribe:    auto subscribe(std::function<void(Event<T>&)> f)
 *                auto subscribe(std::function<void(const T&)> f)
 *  unsubscribe:  void unsubscribe(typename std::list<std::function<void(Event<T>&)>>::iterator &it)
 *  push:         void push(Event<T> &e)
 *                void push(T value)
 *
 * Supported functions:
 *  join:   Stream<T> &join(Stream<T> &s1, Stream<T> &s2)
 *  filter:
 *  map:    Stream<T_dest>* fmap(Stream<T_source> &s, std::function<T_dest(T_source)> f)
 *  fold:
 *
 *  Event callback format:
 *    1) std::function<void(Event<T>&)>
 *       e.g. [&foo](Event<int> &e){foo = e.getValue();}
 *    2) std::function<void(const T&)>
 *       e.g. [&foo](const int &i){foo = i;}
 *
 * TODO: make destructor for deregistering stream
 * from 'joined', 'mapped', or 'filtered' streams
 */
template<typename T>
class Stream : public Dispetcher<Event<T>> {

//  explicit Stream(const Stream &e) = delete;
//  void operator=(const Stream &e) = delete;
//  explicit Stream(const Stream &&e) = delete;
//  void operator=(const Stream &&e) = delete;

 public:
  using fun_t = std::function<void(const T&)>;
  using event_fun_t = typename Dispetcher<Event<T>>::fun_t;

  auto subscribe(fun_t f) {
    struct wrapper_f : Dispetcher<Event<T>>::fun_t {
      Stream::fun_t _f;
      wrapper_f(Stream::fun_t f) {
        _f = f;
      }
      void operator()(Event<T> &e) {
        _f(e.getValue());
      }
    };
    return Dispetcher<Event<T>>::subscribe(wrapper_f(f));
  }

  auto subscribe(event_fun_t f) {
    return Dispetcher<Event<T>>::subscribe(f);
  }

  void push(T value) {
    Event<T> e { value };
    for (auto s : this->subscribers_) {
      s(e);
    }
  }

  void push(Event<T> &e) {
    for (auto s : this->subscribers_) {
      s(e);
    }
  }

  /**
   * friend declarations
   */
  template<typename T1>
  friend Stream<T1>* join(Stream<T1> &s1, Stream<T1> &s2);
};
}  // namespace reacf

#endif /* STREAM_H_ */
