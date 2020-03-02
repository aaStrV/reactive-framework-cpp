#ifndef OBSERVABLE_H_
#define OBSERVABLE_H_

#include <set>
#include <functional>
#include "defines.h"

namespace reacf {

class Observable {
 protected:
  std::set<Observable*> observers_;
  std::set<Observable*> observables_;

 private:
  using it_t = std::set<Observable*>::iterator;

  Observable(Observable &o) = delete;
  Observable(Observable &&o) = delete;
  Observable& operator=(Observable &o) = delete;
  Observable& operator=(Observable &&o) = delete;

 public:
  Observable() {
  }
  ;
  virtual ~Observable() {
  }

//  virtual void removeFromObservables(void);

  virtual void addObservable(Observable *o) {
    observables_.insert(observables_.end(), o);
  }
  virtual void addObserver(Observable *o) {
    observers_.insert(observers_.end(), o);
  }

  virtual void removeObservable(Observable *o) {
    while (true) {
      if (observables_.erase(o) == 0) {
        break;
      }
    }
  }

  virtual void removeObserver(Observable *o) {
    while (true) {
      if (observers_.erase(o) == 0) {
        break;
      }
    }
  }
};

template<typename Dest, typename Source>
class Observer : public Observable {
  std::function<Dest(Source)> f_;

 public:
  virtual ~Observer() {
//    this->removeObserver(this);
//    this->removeObservable(this);
//    delete (&f_);  // TODO: need it? Seems not - falls "free(): invalid pointer"
  }

  Observer(std::function<Dest(Source)> f)
      :
      f_(f) {
//    removeObserver(this);
//    removeObservable(this);
  }

  Dest operator()(Source a) {
    return f_(a);
  }

  virtual void removeObserver(Observable *o) {
    for (Observable *ob : observables_) {
      ob->removeObserver(this);
    }
    Observable::removeObserver(o);
  }

  virtual void removeObservable(Observable *o) {
    for (Observable *ob : observers_) {
      ob->removeObservable(this);
    }
    Observable::removeObservable(o);
  }
};

}  // namespace reacf

#endif /* OBSERVABLE_H_ */
