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

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64

// MSVC defines this in winsock2.h!?
typedef struct reacf__timeval {
    long tv_sec;
    long tv_usec;
} timeval_t;

int gettimeofday(timeval_t * tp, struct timezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}
#else
#include <sys/time.h> // linux
typedef timeval timeval_t;
#endif // _WIN32

namespace reacf {
/**
 * 'Event' is read-only message(any type) with a timestamp
 */
template<typename T>
class Event {
  timeval_t timestamp_;
  const T value_;

  explicit Event(const Event &e) = delete;
  void operator=(const Event &e) = delete;

 public:
  explicit Event(T value)
      :
      value_(value) {
    gettimeofday(&timestamp_, NULL);
  }

  timeval_t getTimeStamp(void) {
    return timestamp_;
  }

  const T& getValue(void) {
    return value_;
  }
};

template<typename T>
class Dispetcher {
 public:
  using it_t = typename std::list<std::function<void(Event<T>&)>>::iterator;
  using event_fun_t = std::function<void(Event<T>&)>;
  using value_fun_t = std::function<void(const T&)>;

 protected:
  std::list<event_fun_t> subscribers_;
  std::multimap<Dispetcher*, it_t> targets_;
  std::set<Dispetcher*> sources_;
  std::string name_;

  explicit Dispetcher(const Dispetcher &e) = delete;
  explicit Dispetcher(const Dispetcher &&e) = delete;
  void operator=(const Dispetcher &e) = delete;
  void operator=(const Dispetcher &&e) = delete;

  /**
   * Remove all links on stream s, setted by joins,
   * mappings, or other functions
   */
  void _removeCallBacks(Dispetcher *s) {
//    std::cout << "_removeCallBacks() in " << this << std::endl;
    while (true) {
      auto it = targets_.find(s);
      if (it != targets_.end()) {
        it_t fun_it = it->second;
        unsubscribe(fun_it);
        targets_.erase(it);
        continue;
      }

      // subclibers clean, delete targets
      if (targets_.erase(s) == 0) {
        break;
      }
    }
  }

  void _sourceWantRemoveIt(Dispetcher *s) {
    sources_.erase(s);
  }

 public:
  Dispetcher(void) {
    subscribers_.clear();
    targets_.clear();
  }

  /**
   * Target stream object should save pointers on his sources.
   * On deleting it say sources to clean it. That means sources
   * should have multimaps <Stream*, iterator>, and '_removeCallbacks(Stream*)'
   * method. Target stream call it as '_removeCallbacks(this)'
   * So, we need Stream._targets:multimap and Stream._sources:multiset
   */
  virtual ~Dispetcher(void) {
    // remove links in source streams
    for (auto ps : sources_) {
      ps->_removeCallBacks(this);
    }

    //remove links in target streams
    for (auto it = targets_.begin(), end = targets_.end(); it != end; ++it) {
      it->first->_sourceWantRemoveIt(this);
    }
  }

  auto subscribe(event_fun_t f) {
    return subscribers_.insert(subscribers_.end(), f);
  }

  auto subscribe(value_fun_t f) {
    struct wrapper_f : event_fun_t {
      value_fun_t _f;
      wrapper_f(value_fun_t f) {
        _f = f;
      }
      void operator()(Event<T> &e) {
        _f(e.getValue());
      }
    };
    return this->subscribe(wrapper_f(f));
  }

  void unsubscribe(it_t &it) {
    subscribers_.erase(it);
//    subscribers_.
  }

  unsigned long size(void) {  // unsigned long is a bad idea
    return static_cast<unsigned long>(subscribers_.size());
  }
};

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
class Stream : public Dispetcher<T> {

//  explicit Stream(const Stream &e) = delete;
//  void operator=(const Stream &e) = delete;
//  explicit Stream(const Stream &&e) = delete;
//  void operator=(const Stream &&e) = delete;

 public:
  Stream(void) {
//    subscribers_.clear();
//    targets_.clear();
//    std::cout << this->subscribers_.size() << std::endl;
  }
//  virtual ~Stream(void) {
//  }

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

/**
 * 'Property'
 *
 * Requirements:
 *  - must be copiable(a = b);
 *
 * Methods:
 *  1) subscribe;   set callback on value changed
 *  2) unsubscribe: unset callback on value changed;
 *  3) set :        let subscribers know changes;
 *
 * TODO: As there must not be read-only events in
 * Property, but copiable values propagation, we need
 * base class Dispetcher for Stream and Property
 */
template<typename T>
class Property : Dispetcher<T> {
  T value_;

 public:
  Property() = delete;
  Property(T value)
      :
      value_(value) {
  }
  Property(Property &value) = delete;
  Property(Property &&value) = delete;
  Property &operator()(Property &value) = delete;
  Property &operator()(Property &&value) = delete;
  T get(void) {
    return value_;
  }

  void set(T v) {
    Event<T> e { value_ };
    for (auto s : this->subscribers_) {
      s(v);
    }
  }
};

}  // namespace reacf
#endif /* REACTIVE_FRAMEWORK_CPP_H_ */
