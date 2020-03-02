#ifndef PROPERTY_H_
#define PROPERTY_H_
#include "Dispetcher.h"

namespace reacf {
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
class Property : ___Dispetcher<T> {
  T value_;

 public:
  Property() = delete;
  Property(T value)
      :
      value_(value) {
  }
  Property(Property &value) = delete;
  Property(Property &&value) = delete;
  Property& operator()(Property &value) = delete;
  Property& operator()(Property &&value) = delete;
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

#endif /* PROPERTY_H_ */
