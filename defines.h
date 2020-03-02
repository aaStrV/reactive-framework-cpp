#ifndef DEFINES_H_
#define DEFINES_H_

#define DEBUG 0
#ifdef DEBUG
  #include <iostream>

  #if DEBUG>0
    #define DPRINT(x) std::cout<<x
    #define DPRINTLN(x) std::cout<<x<<std::endl
  #else
    #define DPRINT(x)
    #define DPRINTLN(x)
  #endif
  #if DEBUG>1
    #define DDPRINT(x) std::cout<<x
    #define DDPRINTLN(x) std::cout<<x<<std::endl
  #else
    #define DDPRINT(x)
    #define DDPRINTLN(x)
  #endif
  #if DEBUG>2
    #define DDDPRINT(x) std::cout<<x
    #define DDDPRINTLN(x) std::cout<<x<<std::endl
  #else
    #define DDDPRINT(x)
    #define DDDPRINTLN(x)
  #endif
#else
  #define DPRINT(x)
  #define DPRINTLN(x)
  #define DDPRINT(x)
  #define DDPRINTLN(x)
  #define DDDPRINT(x)
  #define DDDPRINTLN(x)
#endif

#endif /* DEFINES_H_ */
