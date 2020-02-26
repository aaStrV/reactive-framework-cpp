#ifndef REACTIVE_FRAMEWORK_CPP_H_
#define REACTIVE_FRAMEWORK_CPP_H_
/**
 * Цель проекта: создание системы реактивных объектов.
 *
 * Примерный способ применения:
 *  reacf::Property a(0), b(0), result(0);
 *  result = a + b; // result == 0;
 *  a = 10; // result == 10;
 *  b = 20; // result == 30;
 *
 * Объекты системы должны прозрачно передаваться
 * через разные каналы(например ethernet)
 */
namespace reacf {
class Event {
//  T value_;
};

class Stream{

};
}  // namespace reacf
#endif /* REACTIVE_FRAMEWORK_CPP_H_ */
