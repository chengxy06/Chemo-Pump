/*******************
 *
 * Copyright 1998-2010 IAR Systems AB.  
 *
 * This is a template implementation of the "__write" function used by
 * the standard library.  Replace it with a system-specific
 * implementation.
 *
 * The "__write" function should output "size" number of bytes from
 * "buffer" in some application-specific way.  It should return the
 * number of characters written, or _LLIO_ERROR on failure.
 *
 * If "buffer" is zero then __write should perform flushing of
 * internal buffers, if any.  In this case "handle" can be -1 to
 * indicate that all handles should be flushed.
 *
 * The template implementation below assumes that the application
 * provides the function "MyLowLevelPutchar".  It should return the
 * character written, or -1 on failure.
 *
 ********************/
#ifdef SSZ_TARGET_MACHINE
//#include <yfuns.h>
#include "drv_com.h"
#include "assert.h"


#pragma module_name = "?__write"


/*
 * If the __write implementation uses internal buffering, uncomment
 * the following line to ensure that we are called with "buffer" as 0
 * (i.e. flush) when the application terminates.
 */

size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  /* Remove the #if #endif pair to enable the implementation */

  size_t nChars = size;


  SszFile *p = drv_com_file(kComPCUart);
  assert(p&&p->dev&&p->dev->write_fn);
  for (/* Empty */; size != 0; --size)
  {
  	if(*buffer == '\n'){
	  ssz_fputc(p, '\r');
  	}
    ssz_fputc(p, *buffer);
	buffer++;
  }

  return nChars;
}


#endif