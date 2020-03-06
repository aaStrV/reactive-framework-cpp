/*
 * Distributor.h
 *
 *  Created on: 5 мар. 2020 г.
 *      Author: Sergej_Pechenin
 *
 * Copyright (C) 2020 Sergej_Pechenin.
 *
 * License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */

#ifndef DISTRIBUTOR_H_
#define DISTRIBUTOR_H_
#include <functional>
#include <set>
#include "reactive-framework-cpp.h"

namespace reacf
{
class Object;
class Arrow;
void addIn(Object *o, Arrow *a);
void addOut(Object *o, Arrow *a);
void removeIn(Object *o, Arrow *a);
void removeOut(Object *o, Arrow *a);

class Arrow
{
    Object *dom;
    Object *codom;

public:
    static int count;

    Arrow(Object *dom, Object *codom) :
            dom(dom), codom(codom)
    {
        if (codom != nullptr)
        {
            addIn(codom, this);

        }
        if (dom != nullptr)
        {
            addOut(dom, this);
        }

        count += 1;
    }
    virtual ~Arrow(void)
    {
        if (dom != nullptr)
        {
            removeOut(dom, this);
        }
        if (dom != nullptr)
        {
            removeIn(codom, this);
        }

        count -= 1;
    }

    Object* getDom(void)
    {
        return dom;
    }

    Object* getCodom(void)
    {
        return codom;
    }

    void setDom(Object *o)
    {
        dom = o;
    }

    void setCodom(Object *o)
    {
        codom = o;
    }

    virtual void apply(void*)
    {
    }

    virtual void* retrieve(void)
    {
        return nullptr;
    }

    friend void addIn(Object *o, Arrow *a);
    friend void addOut(Object *o, Arrow *a);
    friend void removeIn(Object *o, Arrow *a);
    friend void removeOut(Object *o, Arrow *a);
};

int Arrow::count = 0;

class Object
{
protected:
    std::set<Arrow*> in_;
    std::set<Arrow*> out_;

public:
    static int count;

    Object()
    {
        count += 1;
    }
    virtual ~Object()
    {
        count -= 1;
        while (true)
        {
            if (in_.size() == 0)
            {
                break;
            }
            Arrow *a = *(in_.begin());
            Object *dom = a->getDom();
            delete a;
//      check(dom);
        }

        while (true)
        {
            if (out_.size() == 0)
            {
                break;
            }
            Arrow *a = *(out_.begin());
            Object *codom = a->getCodom();
            delete a;
//      check(codom);
        }
    }

    bool hasIn(Arrow *a)
    {
        return in_.find(a) == in_.end() ? false : true;
    }

//  bool hasAnyIn(void) {
//    return in_.size() == 0 ? false : true;
//  }

    bool hasOut(Arrow *a)
    {
        return out_.find(a) == out_.end() ? false : true;
    }

//  bool hasOut(void) {
//    return out_.size == 0 ? false : true;
//  }

    virtual void addIn(Arrow *a)
    {
        DDPRINT(this);
        DDPRINT(":addIn adding input ");
        DDPRINTLN(a);
        this->in_.insert(a);
    }

    virtual void addOut(Arrow *a)
    {
        DDPRINT(this);
        DDPRINT(":addOut adding output ");
        DDPRINTLN(a);
        this->out_.insert(a);
    }

    virtual void removeIn(Arrow *a)
    {
        DDPRINT(this);
        DDPRINT(":removeIn removing input ");
        DDPRINTLN(a);
        this->in_.erase(a);
    }

    virtual void removeOut(Arrow *a)
    {
        DDPRINT(this);
        DDPRINT(":removeOut removing output ");
        DDPRINTLN(a);
        this->out_.erase(a);
    }

    virtual void distribute(Arrow *a)
    {
    }

    friend Arrow::~Arrow(void);
};

int Object::count = 0;

void addIn(Object *o, Arrow *a)
{
    o->addIn(a);
}
void addOut(Object *o, Arrow *a)
{
    o->addOut(a);
}
void removeIn(Object *o, Arrow *a)
{
    o->removeIn(a);
}
void removeOut(Object *o, Arrow *a)
{
    o->removeOut(a);
}

/**
 * Morphism lifetime
 *  - Creating: in fmap, filter, subscribe or other functor
 *  - Calling: on publish method from dispetcher
 *  - Deleting: on domain or codomain deletion, on unsubscribe
 */
template<typename Dest, typename Source>
class Morphism: public Arrow
{
public:
    typedef std::function<Dest(Source)> fun_t;

private:
    fun_t f_;
    Dest result_;

public:
    using dom_t = Source;
    using codom_t = Dest;

    Morphism(Object *dom, Object *codom, fun_t f) :
            Arrow(dom, codom), f_(f)
    {
        DDPRINT(this);DDPRINTLN(":Morphism");
    }
    virtual ~Morphism()
    {
        DDPRINT(this);DDPRINTLN(":~Morphism");
    }
//  Dest operator()(Source a) {
//    DDPRINT(this);
//    DDPRINTLN(":Morphism:operator()");
//    return f_(a);
//  }

    virtual void* retrieve(void)
    {
        return &result_;
    }

    virtual void apply(void *a)
    {
        DDDPRINT(this);
        DDDPRINTLN(":Morphism:apply");
        result_ = f_(*((Source*) a));
    }
};

/**
 * Terminal lifetime
 *  - Creating: in subscribe function
 *  - Calling: never
 *  - Deleting: kill itself on removing input
 */
class Terminal: public Object
{
public:
    using terminal_t = int;

    static int count;

public:
    Terminal()
    {
        count += 1;
    }

    ~Terminal()
    {
        count -= 1;
    }

    virtual void addIn(Arrow *a)
    {
    }

    virtual void addOut(Arrow *a)
    {
    }

    virtual void removeIn(Arrow *a)
    {
        delete this;
    }

    virtual void removeOut(Arrow *a)
    {
    }
};

class Initial
{
public:
    static int count;
public:
    Initial()
    {
        count += 1;
    }

    ~Initial()
    {
        count -= 1;
    }
};

int Terminal::count = 0;
int Initial::count = 0;

template<typename T>
class Distributor: public reacf::Object
{
public:
    typedef std::function<void(T)> fun_t;
    static int count;

    virtual ~Distributor()
    {
    }

    template<typename Dest>
    Distributor<Dest>* fmap(std::function<Dest(T)> f)
    {
        Distributor<Dest> *new_dispetcher = new Distributor<Dest>;
        Morphism<Dest, T> *m = new Morphism<Dest, T>(this, new_dispetcher, f);
        return new_dispetcher;
    }

    Distributor<T>* filter(std::function<bool(T)> f, T placeholder)
    {
        Distributor<T> *new_dispetcher = new Distributor<T>;
        std::function<T(T)> filterF = [placeholder, f](T a)
        {
            return f(a) == true ? a : placeholder;
        };
        Morphism<T, T> *m = new Morphism<T, T>(this, new_dispetcher, filterF);
        return new_dispetcher;
    }

    Arrow* subscribe(std::function<void(T)> f)
    {
        std::function<Terminal::terminal_t(T)> terminalF = [f](T value)
        {
            f(value);
            return Terminal::terminal_t(0);
        };
        Terminal *new_terminal = new Terminal;
        Arrow *m = new Morphism<Terminal::terminal_t, T>(this, new_terminal,
                terminalF);
        return m;
    }

    void unsubscribe(Arrow *a)
    {
        delete a;
    }

//  template<typename Dest>
    void publish(T value)
    {
        for (reacf::Arrow *a : out_)
        {
            a->apply((void*) &value);
            a->getCodom()->distribute(a);
        }
    }

    void publishRef(T &value)
    {
        for (reacf::Arrow *a : out_)
        {
            a->apply((void*) &value);
            a->getCodom()->distribute(a);
        }
    }
    virtual void distribute(Arrow *a)
    {
        if (hasIn(a))
        {
            T &tmp_val = *(T*) (a->retrieve());
            publishRef(tmp_val);
        }
        else
        {
            return;
        }
    }
};
}  // namespace reacf

#endif /* DISTRIBUTOR_H_ */
