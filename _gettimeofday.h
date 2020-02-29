/*
 * _gettimeofday.h
 *
 *  Created on: 29 февр. 2020 г.
 *      Author: sergius
 */

#ifndef GETTIMEOFDAY_H_
#define GETTIMEOFDAY_H_

namespace reacf {
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64

// MSVC defines this in winsock2.h!?
typedef struct reacf__timeval {
    long tv_sec;
    long tv_usec;
} timeval_t;

int gettimeofday(timeval_t * tp, struct timezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}
#else
#include <sys/time.h> // linux
typedef timeval timeval_t;
#endif // _WIN32
} // namespace reacf

#endif /* GETTIMEOFDAY_H_ */
