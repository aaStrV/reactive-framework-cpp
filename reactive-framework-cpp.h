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

#include "defines.h"
#include "Distributor.h"
#include "Property.h"

namespace reacf {

}  // namespace reacf
#endif /* REACTIVE_FRAMEWORK_CPP_H_ */
