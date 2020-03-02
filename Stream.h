#ifndef STREAM_H_
#define STREAM_H_

#include "Observable.h"
#include "Event.h"
#include "defines.h"
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
class ___Stream : public ___Dispetcher<Event<T>> {

//  explicit Stream(const Stream &e) = delete;
//  void operator=(const Stream &e) = delete;
//  explicit Stream(const Stream &&e) = delete;
//  void operator=(const Stream &&e) = delete;

 public:
  using fun_t = std::function<void(const T&)>;
  using event_fun_t = typename ___Dispetcher<Event<T>>::fun_t;

  auto subscribe(fun_t f) {
    struct wrapper_f : ___Dispetcher<Event<T>>::fun_t {
      ___Stream ::fun_t _f;
      wrapper_f(___Stream ::fun_t f) {
        _f = f;
      }
      void operator()(Event<T> &e) {
        _f(e.getValue());
      }
    };
    return ___Dispetcher<Event<T>>::subscribe(wrapper_f(f));
  }

  auto subscribe(event_fun_t f) {
    return ___Dispetcher<Event<T>>::subscribe(f);
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
  friend ___Stream <T1>* ___join(___Stream <T1> &s1, ___Stream <T1> &s2);
};

template<typename T>
class Stream : public Dispetcher<Event<T>&> {
 private:

 public:
  using fun_t = std::function<void(const T&)>;
  using event_fun_t = typename Dispetcher<Event<T>&>::fun_t;

  virtual Observable* subscribe(fun_t f) {
    struct wrapper_f : event_fun_t {
      fun_t _f;
      wrapper_f(fun_t f) {
        _f = f;
      }
      void operator()(Event<T> &e) {
        _f(e.getValue());
      }
    };
    return Dispetcher<Event<T>&>::subscribe(wrapper_f(f));
  }

  virtual Observable* subscribe(event_fun_t f) {
    return Dispetcher<Event<T>&>::subscribe(f);
  }

  void pushReference(T &value) {
    Event<T> e { value };
    publishReference(e);
  }

  void push(T value) {
    Event<T> e { value };
    this->publishReference(e);
  }

  void push(Event<T> &e) {
    this->publishReference(e);
  }

  Stream<T>* join(Stream<T> &s) {
    // TODO: what if s.join(s)? Needed tests

    Stream<T> *new_stream = new Stream<T>;

    event_fun_t callbackForThisStream = [new_stream, this](Event<T> &e) {
      new_stream->push(e);
    };

    event_fun_t callbackForArgStream = [new_stream, &s](Event<T> &e) {
      new_stream->push(e);
    };  // equivalent of callbackForThisStream, needed only for debug
    Observable *p_this = this->subscribe(callbackForThisStream);
    p_this->addObserver(new_stream);
    new_stream->addObservable(p_this);

    Observable *p_s = s.subscribe(callbackForArgStream);
    p_s->addObserver(new_stream);
    new_stream->addObservable(p_s);

    return new_stream;
  }

  Stream<T>* filter(std::function<bool(T)> f) {
    Stream<T> *new_stream = new Stream<T>;

    event_fun_t filterF = [new_stream, f](Event<T> &e) {
      if (f(e.getValue()) == true) {
        new_stream->publishReference(e);
      }
    };

    auto p = this->subscribe(filterF);
    p->addObserver(new_stream);
    new_stream->addObservable(p);

    return new_stream;
  }
};
}  // namespace reacf

#endif /* STREAM_H_ */
