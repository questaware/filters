#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#include "build.h"
#include "../util/getline.h"

								/* programmer choices */
#ifndef OFFER_PUSH
#define OFFER_PUSH 0
#endif
#ifndef CHOP_LINES
#define CHOP_LINES 1
#endif
#ifndef FREE_LINES
#define FREE_LINES 0
#endif


#if S_64B_PTR
#define PTRMULT 1
#else
#define PTRMULT 1
#endif





Short   argc_;
Short   argix_;
char ** argv_;
FILE * argip_;

#define MAX_FARG_LEN 200
static Char argline[MAX_FARG_LEN];
#if OFFER_PUSH
static Char * heldarg;					/* one pushed argument */
#endif
#if FREE_LINES
static Char * tofreearg;				/* argument to be freed */
#endif


Char * next_arg(Bool  allocate)

{ Char * res;
  if (argc_ != 0)
  { res = ++argix_ >= argc_ ? null : argv_[argix_ * PTRMULT];
    /* printf("L %d i %d is %s\n", argc_, argix_, res); */
  }
#if OFFER_PUSH
  else  if (heldarg != null)
  { res = heldarg;
    heldarg = null;
  } 
#endif
  else
  { static Lbuf_t linebuf;
    static Char * ln;
    static Vint   lnlenp1;
#if CHOP_LINES
    while (true)
    { if (ln == null or lnlenp1 <= 1)
#endif
      {
#if FREE_LINES
        if (tofreearg != null)
        { free(tofreearg);
          tofreearg = null;
        }
#endif
        ln = ffgetline(&linebuf,argip_);
	if (ln == null)
	  return null;
	if (not allocate)
	  res = ln;
	else
	{ Char * mem = (char*)malloc(strlen(ln));
	  if (mem != null)
	    strcpy(&mem[0], ln);
	  ln = mem;
#if FREE_LINES
	  if (not allocate)
	    tofreearg = ln;
#endif
	}
      }
#if CHOP_LINES
      else
      { ln = &ln[lnlenp1]; 
      }
      while (isspace(*ln) and *ln != 0)
        ++ln;
    { Char * lnn = ln;
      while (not isspace(*lnn) and *lnn != 0)
        ++lnn; 
      lnlenp1 = lnn - ln + 1;
      
      *lnn = 0;
      if (*ln != 0)
        break;
      lnlenp1 = 0;
    }}
#endif
    res = ln;
  }
  return res;
}



#if OFFER_PUSH

Bool push_arg(Char *   arg)

{ if (heldarg != null)
    return false;

  if (arg != argline)
    heldarg = arg;
  else
  { heldarg = malloc(strlen(arg));
    if (heldarg == null)
      return false;
    strcpy(&heldarg[0], arg);
  }
}

#endif
