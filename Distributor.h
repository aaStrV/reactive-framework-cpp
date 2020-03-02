#ifndef DISTRIBUTOR_H_
#define DISTRIBUTOR_H_
#include "reactive-framework-cpp.h"

namespace reacf {
class Object;
class Arrow;
void addIn(Object *o, Arrow *a);
void addOut(Object *o, Arrow *a);
void removeIn(Object *o, Arrow *a);
void removeOut(Object *o, Arrow *a);

class Arrow {
  Object *dom;
  Object *codom;

 public:
  Arrow(Object *dom, Object *codom)
      :
      dom(dom),
      codom(codom) {
    if (codom != nullptr) {
      addIn(codom, this);
    }
    if (dom != nullptr) {
      addOut(dom, this);
    }
    DDPRINT(this);DDPRINTLN(":Arrow");
  }
  virtual ~Arrow(void) {
    if (dom != nullptr) {
      removeOut(dom, this);
    }
    if (dom != nullptr) {
      removeIn(codom, this);
    }

    DDPRINT(this);DDPRINTLN(":~Arrow");
  }

  Object* getDom(void){
    return dom;
  }

  Object* getCodom(void){
    return codom;
  }

  virtual void apply(void*) {
    DDPRINT(this);DDPRINTLN(":Arrow:apply");
  }

  virtual void* retrieve(void) {
    return nullptr;
  }

  friend void addIn(Object *o, Arrow *a);
  friend void addOut(Object *o, Arrow *a);
  friend void removeIn(Object *o, Arrow *a);
  friend void removeOut(Object *o, Arrow *a);
};

class Object {
 protected:
  std::set<Arrow*> in_;
  std::set<Arrow*> out_;

 public:
  Object() {
    DDPRINT(this);DDPRINTLN(":Object");
  }
  virtual ~Object() {
    DDPRINT(this);
    DDPRINT(":~Object there are ");
    DDPRINT(in_.size());
    DDPRINTLN(" inputs");
    while (true) {
      if (in_.size() == 0) {
        break;
      }
      Arrow *a = *(in_.begin());
      DDPRINT(this);
      DDPRINT(":~Object deleting output ");
      DDPRINTLN(a);
      delete a;
    }

    DDPRINT(this);
    DDPRINT(":~Object there are ");
    DDPRINT(out_.size());
    DDPRINTLN(" outputs");
    while (true) {
      if (out_.size() == 0) {
        break;
      }
      Arrow *a = *(out_.begin());
      DDPRINT(this);
      DDPRINT(":~Object deleting output ");
      DDPRINTLN(a);
      delete a;
    }
    DDPRINT(this);DDPRINTLN(":~Object finished");
  }

  bool hasIn(Arrow* a) {
    return in_.find(a) == in_.end()? false: true;
  }

  bool hasOut(Arrow* a) {
    return out_.find(a) == out_.end()? false: true;
  }

  virtual void distribute(Arrow *a) {}

  friend Arrow::~Arrow(void);
  friend void addIn(Object *o, Arrow *a);
  friend void addOut(Object *o, Arrow *a);
  friend void removeIn(Object *o, Arrow *a);
  friend void removeOut(Object *o, Arrow *a);
};

void addIn(Object *o, Arrow *a) {
  DDPRINT(o);
  DDPRINT(":addIn adding input ");
  DDPRINTLN(a);
  o->in_.insert(a);
}
void addOut(Object *o, Arrow *a) {
  DDPRINT(o);
  DDPRINT(":addIn adding output ");
  DDPRINTLN(a);
  o->out_.insert(a);
}
void removeIn(Object *o, Arrow *a) {
  DDPRINT(o);
  DDPRINT(":addIn removing input ");
  DDPRINTLN(a);
  o->in_.erase(a);
}
void removeOut(Object *o, Arrow *a) {
  DDPRINT(o);
  DDPRINT(":addIn removing output ");
  DDPRINTLN(a);
  o->out_.erase(a);
}

/**
 * Morphism lifetime
 *  - Creating: in fmap, filter, subscribe or other functor
 *  - Calling: on publish method from dispetcher
 *  - Deleting: on domain or codomain deletion
 */
template<typename Dest, typename Source>
class Morphism : Arrow {
 public:
  typedef std::function<Dest(Source)> fun_t;

 private:
  fun_t f_;
  Dest result_;

 public:
  using dom_t = Source;
  using codom_t = Dest;

  Morphism(Object *dom, Object *codom, fun_t f)
      :
      Arrow(dom, codom),
      f_(f) {
    DDPRINT(this);DDPRINTLN(":Morphism");
  }
  virtual ~Morphism() {
    DDPRINT(this);DDPRINTLN(":~Morphism");
  }
//  Dest operator()(Source a) {
//    DDPRINT(this);
//    DDPRINTLN(":Morphism:operator()");
//    return f_(a);
//  }

  virtual void* retrieve(void) {
    return &result_;
  }

  virtual void apply(void *a) {
    DDPRINT(this);
    DDPRINTLN(":Morphism:apply");
    result_ = f_(*((Source*) a));
  }
};

class Terminal {
};

class Initial {
};

template<typename T>
class Distributor : public reacf::Object {
 public:
  typedef std::function<void(T)> fun_t;
  virtual ~Distributor() {
  }

  template<typename Dest>
  Distributor<Dest>* fmap(std::function<Dest(T)> f) {
    DPRINT(this);
    DPRINTLN(":fmap");
    Distributor<Dest> *new_dispetcher = new Distributor<Dest>;
    Morphism<Dest, T> *m = new Morphism<Dest, T>(this, new_dispetcher, f);
    return new_dispetcher;
  }

  Distributor<T>* filter(std::function<bool(T)> f, T placeholder) {
    DPRINT(this);
    DPRINTLN(":filter");
    Distributor<T> *new_dispetcher = new Distributor<T>;
    std::function<T(T)> filterF = [placeholder, f](T a) {
      DPRINT("filter:filterF: ");
      DPRINTLN(a);
      return f(a) == true ? a : placeholder;
    };
    Morphism<T, T> *m = new Morphism<T, T>(this, new_dispetcher, filterF);
    return new_dispetcher;
  }

  Distributor<Terminal>* subscribe(std::function<void(T)> f) {
    DPRINT(this);
    DPRINTLN(":subscribe");
    std::function<Terminal(T)> terminalF = [f](T value) {
      f(value);
      return Terminal();
    };
    Distributor<Terminal> *new_dispetcher = new Distributor<Terminal>;
    Morphism<Terminal, T> *m = new Morphism<Terminal, T>(this, new_dispetcher,
                                                         terminalF);
    return new_dispetcher;
  }

  void unsubscribe(Object *o) {
    DPRINT(this);
    DPRINTLN(":unsubscribe");
    delete o;
  }

//  template<typename Dest>
  void publish(T value) {
    DDPRINT(this);
    DDPRINTLN(":publish");
    for (reacf::Arrow *a : out_) {
      DDDPRINT(this);
      DDDPRINTLN(":publish:for: publishing");
      a->apply((void*) &value);
      a->getCodom()->distribute(a);
    }
  }

  void publishRef(T &value) {
    DDPRINT(this);
    DDPRINTLN(":publishRef");
    for (reacf::Arrow *a : out_) {
      DDDPRINT(this);
      DDDPRINTLN(":publishRef:for: publishing");
      a->apply((void*) &value);
      a->getCodom()->distribute(a);
    }
  }
  virtual void distribute(Arrow *a) {
    if (hasIn(a)) {
      T &tmp_val = *(T*) (a->retrieve());
      publishRef(tmp_val);
    } else {
      return;
    }
  }
};
}  // namespace reacf

#endif /* DISTRIBUTOR_H_ */
