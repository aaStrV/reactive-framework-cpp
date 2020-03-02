#ifndef DISPETCHER_H_
#define DISPETCHER_H_

namespace reacf {
template<typename T>
class ___Dispetcher {
 public:
  using it_t = typename std::list<std::function<void(T&)>>::iterator;
  using fun_t = std::function<void(T&)>;

 protected:
  std::list<fun_t> subscribers_;
  std::multimap<___Dispetcher*, it_t> targets_;
  std::set<___Dispetcher*> sources_;
  std::string name_;

  explicit ___Dispetcher(const ___Dispetcher &e) = delete;
  explicit ___Dispetcher(const ___Dispetcher &&e) = delete;
  void operator=(const ___Dispetcher &e) = delete;
  void operator=(const ___Dispetcher &&e) = delete;

  /**
   * Remove all links on stream s, setted by joins,
   * mappings, or other functions
   */
  void _removeCallBacks(___Dispetcher *s) {
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

  void _sourceWantRemoveIt(___Dispetcher *s) {
    sources_.erase(s);
  }

 public:
  ___Dispetcher(void) {
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
  virtual ~___Dispetcher(void) {
    // remove links in source streams
    for (auto ps : sources_) {
      ps->_removeCallBacks(this);
    }

    //remove links in target streams
    for (auto it = targets_.begin(), end = targets_.end(); it != end; ++it) {
      it->first->_sourceWantRemoveIt(this);
    }
  }

  it_t subscribe(fun_t f) {
    return subscribers_.insert(subscribers_.end(), f);
  }

  void unsubscribe(it_t &it) {
    subscribers_.erase(it);
//    subscribers_.
  }

  unsigned long size(void) {  // unsigned long is a bad idea
    return static_cast<unsigned long>(subscribers_.size());
  }

  void _testSet(T a) {
    for (auto s : subscribers_) {
      s(a);
    }
  }

  void _send(T &value) {
    for (auto s : subscribers_) {
      s(value);
    }
  }

  void _insertSource(___Dispetcher <T> *s) {
    sources_.insert(s);
  }

//  template<typename T_dest>
//  Dispetcher<T_dest>* fmap(std::function<T_dest(T&)> f) {
//    Dispetcher<T_dest> *result = new Dispetcher<T_dest>;  // don't delete this!
//
//    auto convertF = [result, f](T &value) -> void {
//      T_dest tmp = f(value);
//      result->_send(tmp);
//    };
//
//    it_t it = this->subscribe(convertF);
//    result->_insertSource(this);
//    //  typename Stream<T_source>::it_t it = s.subscribe(convertEvent);
//    //  result->sources_.insert(std::make_pair(&s, it));
//
//    return result;
//  }
};

/**
 * Dispetcher methods:
 *  - subscribe
 *  - unsubscribe
 *  - publishValue
 *  - publishReference
 */
template<typename T>
class Dispetcher : public Observable {
 protected:

 public:
  using fun_t = std::function<void(T)>;
  virtual ~Dispetcher() {

    // clean observables
    for (Observable *observable_fun : observables_) {
      observable_fun->removeObserver(this);
      delete observable_fun;
    }

    // clean observers
    for (Observable *observer_fun : observers_) {
      observer_fun->removeObservable(this);
      delete observer_fun;
    }
  }

  virtual Observable* subscribe(std::function<void(T)> f) {
    Observer<void, T> *wrapF = new Observer<void, T>(f);  // delete in unsubscribe()

    wrapF->addObservable(this);
    this->addObserver(wrapF);

    return wrapF;
  }

  virtual void unsubscribe(Observable *o) {
    o->removeObservable(this);
    removeObserver(o);
    delete o;  // created in subscribe()
  }

  void publish(T a) {
    for (auto o : observers_) {
      (*((Observer<void, T>*) o))(a);
    }
  }

  void publishReference(T &a) {
    for (auto o : observers_) {
      (*((Observer<void, T>*) o))(a);
    }
  }

  template<typename Dest>
  Dispetcher<Dest>* fmap(std::function<Dest(T)> f) {
    Dispetcher<Dest> *new_dispetcher = new Dispetcher<Dest>;

    fun_t mapperF = [new_dispetcher, f](T a) {
      Dest b = f(a);
      new_dispetcher->publishReference(b);
    };

    auto p = this->subscribe(mapperF);
    p->addObserver(new_dispetcher);
    new_dispetcher->addObservable(p);

    return new_dispetcher;
  }

  Dispetcher<T>* filter(std::function<bool(T)> f) {
    Dispetcher<T> *new_dispetcher = new Dispetcher<T>;

    fun_t filterF = [new_dispetcher, f](T a) {
      if (f(a) == true) {
        new_dispetcher->publishReference(a);
      }
    };

    auto p = this->subscribe(filterF);
    p->addObserver(new_dispetcher);
    new_dispetcher->addObservable(p);

    return new_dispetcher;
  }

  std::list<Observable*>::size_type observersSize(void) {
    return observers_.size();
  }
  std::list<Observable*>::size_type observablesSize(void) {
    return observables_.size();
  }
};
}  // namespace reacf

#endif /* DISPETCHER_H_ */
