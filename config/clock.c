/* Copyright 1998-2010 IAR Systems AB. */


/*
 * This is the default implementation of the "clock" function of the
 * standard library.  It can be replaced with a system-specific
 * implementation.
 *
 * The "clock" function should return the processor time used by the
 * program from some implementation-defined start time.  The value
 * should be such that if divided by the macro CLOCKS_PER_SEC the
 * result should yield the time in seconds.
 *
 * The value "(clock_t)-1" means that the processor time is not
 * available.
 *
 */
#ifdef SSZ_TARGET_MACHINE
#include <time.h>
#include "ssz_tick_time.h"

#pragma module_name = "?clock"

clock_t (clock)(void)
{
  return ssz_tick_time_now();
}


#endif