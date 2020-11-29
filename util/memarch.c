
#include "build.h"

/* this function is highly OS dependant */

Bool prob_ptr(Char * s)


#if S_SOLARIS && 0
{ if (((Int)s & 1) or in_range((Int)s, -1000000, 10000))
    return false;
  return true;
}
#else
{ extern void * sbrk();
  extern char * _start;
  Char * top = sbrk(0L);
  if (s >= top)
    return false;
  if (s < &_start)
    return false;
  return true;
}
#endif
