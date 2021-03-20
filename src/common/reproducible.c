/*****************************************************************************/
/*                                                                           */
/*                               reproducible.c                              */
/*                                                                           */
/*          Support reproducible builds by the cc65 compiler tools           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* Copyright 2021 The cc65 Authors                                           */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#include <limits.h>
#include <stdlib.h>

/* common */
#include "reproducible.h"



/* Decide which integer type to use for converting the time string */
#if defined(__TIMESIZE) /* time symbol handling in the GNU C Library */
#  if (__TIMESIZE == 32)
#    define REPRODUCIBLE_USE_32BIT 1
#  endif
#elif defined(_USE_32BIT_TIME_T) /* 32-bit forced on MSVC */
#  define REPRODUCIBLE_USE_32BIT 1
#elif defined(_MSC_VER)
#  if (_MSC_VER < 1400) /* 64-bit not available before MSVC2005 */
#    define REPRODUCIBLE_USE_32BIT 1
#  endif
#else /* fallback to integer size */
#  define REPRODUCIBLE_TIME_T_SIZE_UNKNOWN 1
#  ifdef LLONG_MAX
     /* 64-bit integer is available */
#  else
#    define REPRODUCIBLE_USE_32BIT 1
#  endif
#endif



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



time_t ReproducibleTime (const time_t Time)
/* If the environment variable SOURCE_DATE_EPOCH is set, use its value as time
** instead.
*/
{
    time_t FinalTime;
    const char * const Env = getenv ("SOURCE_DATE_EPOCH");

    if (Env == NULL) {
        FinalTime = Time;
    } else {
#if defined(REPRODUCIBLE_USE_32BIT)
        const long Epoch = strtol (Env, NULL, 10);
#else
        const long long Epoch = strtoll (Env, NULL, 10);
#endif

#if defined(REPRODUCIBLE_TIME_T_SIZE_UNKNOWN) && ! defined(REPRODUCIBLE_USE_32BIT)
        if ((sizeof (time_t) == 4) && (Epoch > LONG_MAX)) {
            FinalTime = LONG_MAX;
        } else {
            FinalTime = (time_t) Epoch;
        }
#else
#error
        /* no cast, expected to be compatible */
        FinalTime = Epoch;
#endif
    }

    return FinalTime;
}
