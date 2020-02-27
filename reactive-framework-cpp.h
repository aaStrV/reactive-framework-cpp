#ifndef REACTIVE_FRAMEWORK_CPP_H_
#define REACTIVE_FRAMEWORK_CPP_H_
/**
 * Цель проекта: создание системы реактивных объектов.
 *
 * Примерный способ применения:
 *  reacf::Property a(0), b(0), result(0);
 *  result = a + b; // result == 0;
 *  a = 10; // result == 10;
 *  b = 20; // result == 30;
 *
 * Объекты системы должны прозрачно передаваться
 * через разные каналы(например ethernet)
 */

#include <list>
#include <functional>
//#include <iterator>

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

  T getValue(void) {
    return value_;
  }
};

/**
 * Методы:
 *  subscribe( fun(event<T> e) ), unSubscribe( fun(event<T> e) ); fun:lambda
 *  push(event<T> e)
 *
 * Поддерживаемые функции:
 *  Stream<T> &join(Stream<T> &s1, Stream<T> &s2)
 *  filter
 *  map
 *  fold
 */
template<typename T>
class Stream {
  std::list<std::function<void(Event<T>&)>> subscribers_;
//  std::list<typename std::list<std::function<void(Event<T>&)>>::iterator> sources_;

  explicit Stream(const Stream &e) = delete;
  void operator=(const Stream &e) = delete;

 public:

  Stream(void) = default;

  auto subscribe(std::function<void(Event<T>&)> f) {
    return subscribers_.insert(subscribers_.end(), f);
  }

  void unsubscribe(
      typename std::list<std::function<void(Event<T>&)>>::iterator &it) {
    subscribers_.erase(it);
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

};

/**
 * Функция создает новый поток из двух исходных потоков,
 * устанавливая в них обработчики событий
 */
template<typename T>
Stream<T>& join(Stream<T> &s1, Stream<T> &s2) {
  Stream<T> *ps = new Stream<T>;  // don't delete this!
  Stream<T> &s = *ps;
  auto connectStream = [&s](Event<T> &e) -> void {
    s.push(e);
  };
  s1.subscribe(connectStream);
  s2.subscribe(connectStream);
  return s;
}

/**
 * Argument function returns copy of new event
 */
template<typename T_source, typename T_dest>
Stream<T_dest>& map(Stream<T_source> &s,
                    std::function<Event<T_dest>(Event<T_source>&)> f) {
}

/**
 * Argument function returns reference to new event
 */
template<typename T_source, typename T_dest>
Stream<T_dest>& map(Stream<T_source> &s,
                    std::function<Event<T_dest>& (Event<T_source>&)> f) {
}
}  // namespace reacf
#endif /* REACTIVE_FRAMEWORK_CPP_H_ */
