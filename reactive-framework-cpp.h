/*
 * reactive-framework-cpp.h
 *
 *  Created on: 5 мар. 2020 г.
 *      Author: Sergej_Pechenin
 *
 * Copyright (C) 2020 Sergej_Pechenin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef REACTIVE_FRAMEWORK_CPP_H_
#define REACTIVE_FRAMEWORK_CPP_H_

/**
 * Target
 *    Get a system of reactive objects.
 *
 *    Assume something like this:
 *      reacf::Property a(0), b(0), result(0), c(0);
 *      result = a + b; // result == 0;
 *      a = 10; // result == 10;
 *      c = a; // c == 10
 *      b = 20; // result == 30, c == 30;
 *
 * Requirements
 *    - System should support distributed applications development;
 */

#include <functional>
#include <set>
#include "defines.h"
//#include "distributor.h"
//#include "property.h"

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
        this->in_.insert(a);
    }

    virtual void addOut(Arrow *a)
    {
        this->out_.insert(a);
    }

    virtual void removeIn(Arrow *a)
    {
        this->in_.erase(a);
    }

    virtual void removeOut(Arrow *a)
    {
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
    }
    virtual ~Morphism()
    {
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

/**
 * 'Property'
 */
template<typename T>
class Property: Distributor<T>
{
    T value_;
    Morphism<Terminal::terminal_t, T> *self_;

    void _subscribeSelf(void)
    {
        std::function<Terminal::terminal_t(T)> getF = [this](T a)
        {
            this->value_ = a;
            return Terminal::terminal_t(0);
        };

        Terminal *new_terminal = new Terminal;
        Morphism<Terminal::terminal_t, T> *m = new Morphism<
                Terminal::terminal_t, T>(this, new_terminal, getF);
        self_ = m;
    }

    void _unsubscribeSelf()
    {
//        this->unsubscribe(self_);
        if (self_ != nullptr)
        {
            delete self_;
        }
    }

public:
    Property(void)
    {
        this->_subscribeSelf();
    }
    virtual ~Property()
    {
//        _unsubscribeSelf();
    }
    Property(T a)
    {
        _subscribeSelf();
        this->publishRef(a);
    }
    Property(Property &p)
    {
        _subscribeSelf();
        Morphism<T, T> *m = new Morphism<T, T>(&p, this, [](T a)
        {   return a;});
    }
    Property(Property &&p)
    {
        _unsubscribeSelf();
        p._unsubscribeSelf();

        std::swap(this->in_, p.in_);
        std::swap(this->out_, p.out_);

        for (Arrow *a : this->in_)
        {
            a->setCodom(this);
        }

        for (Arrow *a : this->out_)
        {
            a->setDom(this);
        }

        _subscribeSelf();
        this->publishRef(p.value_);
    }

    Property& operator=(Property &p)
    {
        if (this == &p)
        {
            return *this;
        }

        Morphism<T, T> *m = new Morphism<T, T>(&p, this, [](T a)
        {   return a;});
        this->publishRef(p.value_);

        return *this;
    }

    Property& operator=(Property &&p)
    {
        if (this == &p)
        {
            return *this;
        }

        _unsubscribeSelf();
        p._unsubscribeSelf();

        this->in_.swap(p.in_);
        this->out_.swap(p.out_);

        for (auto it = this->in_.begin(), end = this->in_.end(); it != end;
                ++it)
        {
            (*it)->setCodom(this);
        }

        for (auto it = this->out_.begin(), end = this->out_.end(); it != end;
                ++it)
        {
            (*it)->setDom(this);
        }

        _subscribeSelf();
        this->publishRef(p.value_);

        return *this;
    }

    Property& operator=(T b)
    {
        this->publish(b);
        return *this;
    }

    T& getValue(void)
    {
        return value_;
    }

    Property operator+(Property &arg)
    {
        Property p;
        Property *p_arg = &arg;

        new Morphism<T, T>(this, &p, [p_arg](T a)
        {
            return a + p_arg->value_;
        });

        new Morphism<T, T>(p_arg, &p, [this](T a)
        {
            return a + this->value_;
        });

        T tempval = arg.value_ + this->value_;
        p.publishRef(tempval);

        return p;
    }

    Property operator-(Property &arg)
    {
        Property p;
        Property *p_arg = &arg;

        new Morphism<T, T>(this, &p, [p_arg](T a)
        {
            return a - p_arg->value_;
        });

        new Morphism<T, T>(p_arg, &p, [this](T a)
        {
            return this->value_ - a;
        });

        T tempval = this->value_ - arg.value_;
        p.publishRef(tempval);

        return p;
    }

    Property operator*(Property &arg)
    {
        Property p;
        Property *p_arg = &arg;

        new Morphism<T, T>(this, &p, [p_arg](T a)
        {
            return a * p_arg->value_;
        });

        new Morphism<T, T>(p_arg, &p, [this](T a)
        {
            return this->value_ * a;
        });

        T tempval = this->value_ * arg.value_;
        p.publishRef(tempval);

        return p;
    }

    Property operator/(Property &arg)
    {
        Property p;
        Property *p_arg = &arg;

        new Morphism<T, T>(this, &p, [p_arg](T a)
        {
            return a / p_arg->value_;
        });

        new Morphism<T, T>(p_arg, &p, [this](T a)
        {
            return this->value_ / a;
        });

        T tempval = this->value_ / arg.value_;
        p.publishRef(tempval);

        return p;
    }

    Property operator%(Property &arg)
    {
        Property p;
        Property *p_arg = &arg;

        new Morphism<T, T>(this, &p, [p_arg](T a)
        {
            return a % p_arg->value_;
        });

        new Morphism<T, T>(p_arg, &p, [this](T a)
        {
            return this->value_ % a;
        });

        T tempval = this->value_ % arg.value_;
        p.publishRef(tempval);

        return p;
    }
};
}  // namespace reacf
#endif /* REACTIVE_FRAMEWORK_CPP_H_ */
