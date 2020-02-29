/*
 * Event.h
 *
 *  Created on: 29 февр. 2020 г.
 *      Author: sergius
 */

#ifndef EVENT_H_
#define EVENT_H_
#include "_gettimeofday.h"

namespace reacf{
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
} // namespace reacf




#endif /* EVENT_H_ */
