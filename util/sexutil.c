#include   "build.h"
#include   "sexutil.h"

/*
  MEM_TO_PRTABLE: Convert a string to (an ambiguious) printable representation.
 */
public Char * mem_to_prtable(t_, s_, n)
	Char   * t_;
	Char *   s_;
	Short    n;
{	   Char * to_hexdig = "0123456789abcdef";
  register Char * t = t_;
  register Char * s = s_;
  
  while (--n > 0)
    if (in_range(*s, ' ', 127))
      *t++ = *s++;
    else
    { *t++ = '^';
      *t++ = to_hexdig[(*s >> 4) & 0xf];
      *t++ = to_hexdig[*s & 0xf];
    }
  *t = 0;
  return t_;
}

