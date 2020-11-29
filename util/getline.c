#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>


#include "build.h"

#include "getline.h"
#include "../common/h/eprintf.h"

#if 1

#define mymalloc(n) (Char*)malloc(n)
#define myfree(t) free(t)

#else

Char *lm1, *lm2, *lm3;

Char * mymalloc(n)
	Int  n;
{ extern Char * malloc();
  Char * res = malloc(n);
  lm3 = lm2;
  lm2 = lm1;
  lm1 = res;
#if 0
  printf("Malloc %lx\n", res);
  if (n > 120)
    printf("SIZE %lx\n", n);
#endif
  return res;
}


void myfree(t)
	Char * t;
{ eprintf(null, t != lm1 and t != lm2 and t != lm3 ? "Wrong free %lx\n"
						   : "Good free %lx\n", t);
  free(t);
}

#endif

Char * ffgetline(
	Lbuf      lbuf,
	FILE *    fd)
{	register Vint c; 	/* current character read */
	register Vint i = -1; 	/* current index into dif_line */
		 Vint skipsp = lbuf->props & M_SKIPSP;
	         Char * dif_line = lbuf->line;	/* cache */

        if (lbuf->avail > 3 * NSTRING)		/* free big strings */
	  lbuf->avail = 0;

	lbuf->len = 0;
	while (true)
	{ if (++i >= lbuf->avail)
	  { if (i > 20 * NSTRING)
	      break;
	  { char * tmpline = (char*)mymalloc((Int)lbuf->avail+NSTRING+1);
	    if (tmpline == NULL)
	    { fputserr("Out of Mem");
	      return null;
	    }
	    if (dif_line != null)
	    { memcpy(tmpline, dif_line, lbuf->avail+1);
	      myfree(dif_line);
	    }
	    lbuf->line = dif_line = tmpline;
	    lbuf->avail += NSTRING;
	  }}
	  if ((c = getc(fd)) < OK or c == '\n')
	    break;

	  if (skipsp)
	  { if (isspace(c))
	    { --i;
	      continue;
	    }
	    skipsp = false;
	  }
	  				    /* if it's longer, get more room */
	  dif_line[i] = c;
	}

	  
        lbuf->len = i;
	dif_line[i] = 0;
        if (i > 0 && dif_line[i-1] == 0xd)
	  dif_line[i-1] = 0;

	return i == 0 and c == EOF ? null : dif_line;
}

