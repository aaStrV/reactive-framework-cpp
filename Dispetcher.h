/*
 * Dispetcher.h
 *
 *  Created on: 29 февр. 2020 г.
 *      Author: sergius
 */

#ifndef DISPETCHER_H_
#define DISPETCHER_H_

namespace reacf{
template<typename T>
class Dispetcher {
 public:
  using it_t = typename std::list<std::function<void(T&)>>::iterator;
  using fun_t = std::function<void(T&)>;

 protected:
  std::list<fun_t> subscribers_;
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
};
} // namespace reacf



#endif /* DISPETCHER_H_ */
