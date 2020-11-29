#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "build.h"
#include "getline.h"
#include "../common/h/eprintf.h"


#define M_VERBOSE 1
#define M_SKIPSP  2
#define M_IFTHERE 4			/* also defined in stree.c */


#undef NSTRING
#define NSTRING 50

#define SMALLINE   50
#define BIGLINE   500
#define LINEINCR   50

Vint   dif_flags;


Vint strsigcmp(Char * s1,
	       Char * s2)
{ fast Vint ch;

  while (true)
  { ch = *s1;
    if      (isspace(ch))
      ;
    else if (isspace(*s2))
    { ++s2;
      continue;
    }
    else
    { if (ch != *s2++)
        return 1;
      if (ch == 0)
        return 0;
    }
    ++s1;
  }
}

Lbuf_t linebuf1, linebuf2;

					/* -1 => file 1 not found */
					/* -2 => file 2 not found */
					/* 1 => different */
					/* OK => same */
Cc same_files(
	Char *   fname1,
	Char *   fname2,
	Vint     flags)
{ Cc cc = OK;
  Vint lno = 0;
  FILE * fd1 = fopen(fname1, "rb");
  if (fd1 == null)
    return -1;

{ FILE * fd2 = fopen(fname2, "rb");
  if (fd2 == null)
    cc = -2;
  else
  { linebuf1.props = flags;
    linebuf2.props = flags;

    while (true)
    { ++lno;
    { Char * line1 = ffgetline(&linebuf1, fd1);
      Char * line2;

      if      (line1 == null)
        ;
      else if ((flags & M_SKIPSP) and *line1 == 0)
        continue;

      while (true)
      { line2 = ffgetline(&linebuf2, fd2);
        if (line2 == null or not (flags & M_SKIPSP) or *line2 != 0)
          break;
      }

      if (line1 == line2)			/* => both == null */
        break;

      if (line1 == null or line2 == null or
       ((flags & M_SKIPSP ? strsigcmp(line1,line2) : strcmp(line1,line2)) != 0))
      { if (cc == OK)
        { cc = 1;
          if (flags & M_VERBOSE) 
            eprintf(null, "XXXXX %s\n", fname1);
        }
        if (flags & M_VERBOSE)
        { eprintf(null, line1 == null ? "%#%l#$\n> %s\n" :
                        line2 == null ? "%#$\n< %s\n"   :
                                        "%d\n< %s\n> %s\n", lno, line1, line2);
        }
        break;
      }
    
  /*  printf("F1 %s\n", line1); */
  /*  printf("F2 %s\n", line2); */
    }}

    fclose(fd2);
  }
  fclose(fd1);
  return cc;
}}
