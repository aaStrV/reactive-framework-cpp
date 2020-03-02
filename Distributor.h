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
    DDPRINT(this);
    DDPRINTLN(":Arrow");
  }
  virtual ~Arrow(void) {
    if (dom != nullptr) {
      removeOut(dom, this);
    }
    if (dom != nullptr) {
      removeIn(codom, this);
    }

    DDPRINT(this);
    DDPRINTLN(":~Arrow");
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
    DDPRINT(this);
    DDPRINTLN(":Object");
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
    DDPRINT(this);
    DDPRINTLN(":~Object finished");
  }
  friend Arrow::~Arrow(void);
  friend void addIn(Object *o, Arrow *a);
  friend void addOut(Object *o, Arrow *a);
  friend void removeIn(Object *o, Arrow *a);
  friend void removeOut(Object *o, Arrow *a);
};

void addIn(Object *o, Arrow *a) {
  o->in_.insert(a);
}
void addOut(Object *o, Arrow *a) {
  o->out_.insert(a);
}
void removeIn(Object *o, Arrow *a) {
  o->in_.erase(a);
}
void removeOut(Object *o, Arrow *a) {
  o->out_.erase(a);
}

template<typename Dest, typename Source>
class Morphism : Arrow {
 public:
  typedef std::function<Dest(Source)> fun_t;

 private:
  fun_t f_;

 public:
  Morphism(Object *dom, Object *codom, fun_t f)
      :
      Arrow(dom, codom),
      f_(f) {
    DDPRINT(this);
    DDPRINTLN(":Morphism");
  }
  virtual ~Morphism() {
    DDPRINT(this);
    DDPRINTLN(":~Morphism (nothing to do)");
  }
  Dest operator()(Source a) {
    DDPRINT(this);
    DDPRINTLN(":Morphism:operator()");
    f_(a);
  }
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
    Morphism<Dest, T> m(new_dispetcher, this, f);
    return new_dispetcher;
  }

  Object* subscribe(fun_t f) {
    DPRINT(this);
    DPRINTLN(":subscribe");
    Object *new_object = new Object;
    Morphism<void, T> *m = new Morphism<void, T>(this, new_object, f);
    return new_object;
  }

  void unsubscribe(Object *o) {
    DPRINT(this);
    DPRINTLN(":unsubscribe");
    delete o;
  }

  void publish(T value) {
    DDPRINT(this);
    DDPRINTLN(":publish");
    for (reacf::Arrow *a : out_) {
      DDDPRINT(this);
      DDDPRINTLN(":publish:for: publishing");
      (*((reacf::Morphism<void, T>*) a))(value);
    }
  }

  void publishRef(T &value) {
    DDPRINT(this);
    DDPRINTLN(":publishRef");
    for (reacf::Arrow *a : out_) {
      (*((reacf::Morphism<void, T>*) a))(value);
    }
  }
};
}  // namespace reacf

#endif /* DISTRIBUTOR_H_ */
