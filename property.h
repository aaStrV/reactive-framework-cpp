/*
 *              Property.h
 *
 * Created on:  5 mar 2020
 * Author:      Sergej_Pechenin
 *
 *              Copyright (C) 2020 Sergej_Pechenin.
 *
 *              License GPLv3+: GNU GPL version 3 or later
 *              <https://gnu.org/licenses/gpl.html>
 *              This is free software: you are free to change
 *              and redistribute it. There is NO WARRANTY,
 *              to the extent permitted by law.
 */

#ifndef PROPERTY_H_
#define PROPERTY_H_
#include "distributor.h"

namespace reacf
{
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
}
// namespace reacf

#endif /* PROPERTY_H_ */
