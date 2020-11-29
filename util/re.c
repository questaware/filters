#include   <stdio.h>
#include   <ctype.h>
#include   <sys/types.h>
#include   "build.h"


/* if the pattern matches at tlt_, return the point at which it ends 
   otherwise tlt_
*/


Char * match_re(
	Char *   tlt_,
	Char *   pat_,
	Bool     ic)
{	   Char * pat = pat_;
  register Char * tlt = tlt_;
	   Char * rpt = tlt;
	   Short repeat = 0;
           Bool pass = true;

  while (*pat != 0)
  { Char * spat = pat;

    pass = *tlt != 0;
    if      (*pat == '.')
      ;
    else if (*pat == '$' && pat[1] == 0)
    { 
      pass = *tlt == 0;
      break;
    }
    else if (*pat == '^')
    { Char pch = *++pat;
      if ((in_range(pch, '@','Z') ? pch-'@'   :
	   in_range(pch, 'a','z') ? pch-'a'+1 : pch) != *tlt)
	pass = false;
    }
    else if (*pat == '[')
    { Bool compl_set = *++pat == '^';
      pass &= compl_set;

      if (not compl_set)
        --pat;
      while (*++pat != 0)
        if (*pat == '-' && pat[-1] != '[' ? in_range(*tlt,pat[-1],pat[1])
              				  : *tlt == *pat)
          pass = not compl_set;
        else if (*pat == ']' && pat[-1]!='[' && (pat[-1]!='^' || pat[-2]!='['))
          break;
    }
    else if (not ic ? *pat == *tlt : toupper(*pat) == toupper(*tlt))
      ;
    else
      pass = false;
    
    ++pat;
    if (pass)
    { repeat += 1;
      if      (*pat == '*' or *pat == '+')
        pat = spat;
      else
      { repeat = 0;
        if (*pat == '?')
	  ++pat;
      }
    }
    else
    { if      (*pat == '*' or *pat == '?')
        ;
      else if (*pat == '+')
      { if (repeat == 0)
          break;
      }
      else
	break;
      repeat = 0;
      ++pat;
      if (*pat == 0)
        pass = true;
      if (not pass)
        continue; 
    }
    if (*tlt != 0)
      ++tlt;      
  }

  return not pass ? tlt_ : tlt;
}

