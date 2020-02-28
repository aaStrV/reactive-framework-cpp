#ifndef REACTIVE_FRAMEWORK_CPP_H_
#define REACTIVE_FRAMEWORK_CPP_H_
/**
 * Target
 *    Get a system of reactive objects.
 *
 *    Assume something like this:
 *      reacf::Property a(0), b(0), result(0);
 *      result = a + b; // result == 0;
 *      a = 10; // result == 10;
 *      b = 20; // result == 30;
 *
 * Requirements
 *    - System should support distributed applications development;
 */
//#define DEBUG
#include <list>
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
class Stream {
  std::list<std::function<void(Event<T>&)>> subscribers_;
//  std::list<typename std::list<std::function<void(Event<T>&)>>::iterator> sources_;
  std::string name_;

  explicit Stream(const Stream &e) = delete;
  void operator=(const Stream &e) = delete;

 public:

  explicit Stream(std::string name = "") {
#ifdef DEBUG
    name_ = name;
#endif
    subscribers_.clear();
#ifdef DEBUG
    std::cout << "New stream created: '" << name_ << "'" << std::endl;
#endif
  }

  auto subscribe(std::function<void(Event<T>&)> f) {
#ifdef DEBUG
    std::cout << "New subscriber in '" << name_ << "'" << std::endl;
    std::cout << "'" + name_ + "'.size() = " << size()+1 << std::endl;
#endif
    return subscribers_.insert(subscribers_.end(), f);
  }

  auto subscribe(std::function<void(const T&)> f) {
#ifdef DEBUG
    std::cout << "New subscriber in '" << name_ << "'" << std::endl;
    std::cout << "'" + name_ + "'.size() = " << size()+1 << std::endl;
#endif
    struct wrapper_f : std::function<void(Event<T>&)> {
      typename std::function<void(const T&)> _f;
      wrapper_f(std::function<void(const T&)> f) {
        _f = f;
      }
      void operator()(Event<T> &e) {
        _f(e.getValue());
      }
    };
    return this->subscribe(wrapper_f(f));
  }

  void unsubscribe(
      typename std::list<std::function<void(Event<T>&)>>::iterator &it) {
    subscribers_.erase(it);
#ifdef DEBUG
    std::cout << "Subscriber gone from '" << name_ << "'" << std::endl;
    std::cout << "'" + name_ + "'.size() = " << size() << std::endl;
#endif
//    subscribers_.
  }

  void push(T value) {
    Event<T> e { value };
    for (auto s : subscribers_) {
      s(e);
    }
  }

  void push(Event<T> &e) {
    for (auto s : subscribers_) {
      s(e);
    }
  }

  unsigned long size(void) {  // unsigned long is a bad idea
    return static_cast<unsigned long>(subscribers_.size());
  }

#ifdef DEBUG
  std::string getName() {
    return name_;
  }
#endif

};

/**
 * 'join' make a new stream type A from 2 type A streams
 */
template<typename T>
Stream<T>* join(Stream<T> &s1, Stream<T> &s2) {
#ifdef DEBUG
  Stream<T> *result = new Stream<T>(s1.getName() + "+" + s2.getName());  // don't delete this!
#else
  Stream<T> *result = new Stream<T>;  // don't delete this!
#endif
  auto connectStream = [result](Event<T> &e) -> void {
    (*result).push(e);
  };
  s1.subscribe(connectStream);
  s2.subscribe(connectStream);
  return result;
}

/**
 * 'fmap' reflects source stream, type A, on a new stream, type B.
 * E.g. Stream<int> -> Stream<bool>. Needs function that convert type A element
 * to type B element
 */
template<typename T_source, typename T_dest>
Stream<T_dest>* fmap(Stream<T_source> &s, std::function<T_dest(T_source)> f) {
#ifdef DEBUG
  Stream<T_dest> *result = new Stream<T_dest>(s.getName() + "->");  // don't delete this!
#else
  Stream<T_dest> *result = new Stream<T_dest>;  // don't delete this!
#endif

  auto convertEvent = [result, f](Event<T_source> &e) -> void {
    (*result).push(f(e.getValue()));
  };
  s.subscribe(convertEvent);
  return result;
}

/**
 * 'filter'
 */
template<typename T>
Stream<T>* filter(Stream<T> &s, std::function<bool(const T&)> f) {
#ifdef DEBUG
  Stream<T> *result = new Stream<T>(s.getName() + "|");  // don't delete this!
#else
  Stream<T> *result = new Stream<T>;  // don't delete this!
#endif

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
 */

}  // namespace reacf
#endif /* REACTIVE_FRAMEWORK_CPP_H_ */
