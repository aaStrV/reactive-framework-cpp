/*
 * defines.h
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
