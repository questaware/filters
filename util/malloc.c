#include   "build.h"


#if COUNT_MEM
#define MEM_OFFS 4
#else
#define MEM_OFFS 0
#endif

#undef malloc
#undef free

extern Char * malloc();

Int ct_malloc;


public Char * chk_malloc(n)
	Int  n;
{ Char * res = malloc(n + MEM_OFFS);
  if (res == null)
    abort("Out of Memory");

#if COUNT_MEM
  *((Int*)m) = n;
#endif
  ct_malloc += n;
  return &res[MEM_OFFS];  
}




public void chk_free(m)
	Char * m;
{ 
#if COUNT_MEM
  m -= MEM_OFFS;
  ct_malloc -= *((Int*)m);
#endif
  free(m);
}

