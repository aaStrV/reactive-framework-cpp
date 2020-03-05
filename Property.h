/*
 * Property.h
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

#ifndef PROPERTY_H_
#define PROPERTY_H_
#include "Distributor.h"

namespace reacf
{
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
 */
template<typename T>
class Property
{
	T *value_;
	Distributor<T> *distributor_;

//  Property(Property &value) = delete;
	Property(Property &&value) = delete;
////  Property& operator=(Property &value) = delete;
//  Property& operator=(Property &&value) = delete;

	void _setValue(T value)
	{
		*value_ = value;  // do not publish here!!
	}

public:
	Property()
	{
		value_ = new T;
		distributor_ = new Distributor<T>;
		auto setterF = [this](T a) 
		{
			this->_setValue(a);
		};
		distributor_->subscribe(setterF);
	}

	Property(T value)
	{
		value_ = new T(value);
		distributor_ = new Distributor<T>;
		auto setterF = [this](T a) 
		{
			this->_setValue(a);
		};
		distributor_->subscribe(setterF);
	}

	Property(Property &value)
	{
		value_ = new T(value.getValue());
		distributor_ = new Distributor<T>;
		auto setterF = [this](T a) 
		{
			this->_setValue(a);
		};
		distributor_->subscribe(setterF);

		auto copyF = [this, &value](T a) 
		{
			this->distributor_->publish(a);
		};
		value.distributor_->subscribe(copyF);
	}

	Property& operator=(Property &value)
	{
		if (this == &value)
		{
			return *this;
		}

		*value_ = value.getValue();

		auto setterF = [this](T a) 
		{
			this->_setValue(a);
		};
		distributor_->subscribe(setterF);

		auto copyF = [this, &value](T a) 
		{
			this->distributor_->publish(a);
		};
		value.distributor_->subscribe(copyF);

		DDPRINTLN("Property:operator=, something copied");
		return *this;
	}

	Property& operator=(Property &&value)
	{
		if (this == &value)
		{
			return *this;
		}

//    Distributor<T> *tmp_d = value.distributor_;
//    T *tmp_v = value.value_;
//
//    value.distributor_ = tmp_d;
//    value.value_ = tmp_v;

//    std::swap(value_, value.value_);
//    std::swap(distributor_, value.distributor_);

		DDPRINTLN("Property:operator=, something moved");
		return *this;
	}

	Property& operator=(T b)
	{
		// TODO: publish here
//    _setValue(b);
		publish(b);
		return *this;
	}

	T getValue(void)
	{
		return *value_;
	}

	void subscribe(std::function<void(T)> f)
	{
		distributor_->subscribe(f);
	}

	void publish(T a)
	{
		distributor_->publish(a);
	}

	~Property()
	{
		delete distributor_;
	}

	Property<T>& operator+(Property<T> &arg)
	{
		Property<T> &p = *(new Property<T> );
		Property<T> *self = this;

		auto thisF = [self, &p](T a) 
		{
			p.publish(self->getValue() + a);
		};

		auto argF = [&arg, &p](T a) 
		{
			p.publish(arg.getValue() + a);
		};

		arg.subscribe(thisF);
		subscribe(argF);

		return p;
	}

	Property<T>& operator-(Property<T> &arg)
	{
		Property<T> &p = *(new Property<T> );
		Property<T> *self = this;

		auto thisF = [self, &p](T a) 
		{
			p.publish(self->getValue() - a);
		};

		auto argF = [&arg, &p](T a) 
		{
			p.publish(arg.getValue() - a);
		};

		arg.subscribe(thisF);
		subscribe(argF);

		return p;
	}

	Property<T>& operator*(Property<T> &arg)
	{
		Property<T> &p = *(new Property<T> );
		Property<T> *self = this;

		auto thisF = [self, &p](T a) 
		{
			p.publish(self->getValue() * a);
		};

		auto argF = [&arg, &p](T a) 
		{
			p.publish(arg.getValue() * a);
		};

		arg.subscribe(thisF);
		subscribe(argF);

		return p;
	}

	Property<T>& operator/(Property<T> &arg)
	{
		Property<T> &p = *(new Property<T> );
		Property<T> *self = this;

		auto thisF = [self, &p](T a) 
		{
			p.publish(self->getValue() / a);
		};

		auto argF = [&arg, &p](T a) 
		{
			p.publish(arg.getValue() / a);
		};

		arg.subscribe(thisF);
		subscribe(argF);

		return p;
	}

	Property<T>& operator%(Property<T> &arg)
	{
		Property<T> &p = *(new Property<T> );
		Property<T> *self = this;

		auto thisF = [self, &p](T a) 
		{
			p.publish(self->getValue() % a);
		};

		auto argF = [&arg, &p](T a) 
		{
			p.publish(arg.getValue() % a);
		};

		arg.subscribe(thisF);
		subscribe(argF);

		return p;
	}
};
}  // namespace reacf

#endif /* PROPERTY_H_ */
