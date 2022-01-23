#include  <ctype.h>
#include  <string.h>
#include  "build.h"
#include  "grutil.h"

#include  "../common/h/eprintf.h"

extern Char * match_re __((Char *, Char *, Bool));

                                     /* 1) + from first non-blank */
                                     /* 2) <sp>:anywhere, 4) X: ignore case */
extern Vint exc_opt;
extern Char exc_str[120];


				/* also in stree.c */
#define M_NOT 2
#define M_IC  4
#define M_MATCHWORD 8


#if S_MSDOS
#ifndef _DEBUG
/*
void printf()

{ fputserr("PRINTF");
}
*/
void fprintf()

{ fputserr("FPRINTF");
}

#endif
#endif

Cc grep_line(
	Char *   line,
	Char *   pat,
	Byte     props)

{ Bool anchored = *pat == '^';
  if (anchored)
    ++pat;

  if (exc_opt != 0)
  { Int len = strlen(exc_str);
    Char * s = line - 1;
    if (exc_opt & 1)
    { for (; *++s != 0 && *s <= ' '; )
      --s;
    }
    
    if ((exc_opt & 2) == 0)
      if (exc_opt & 4)
      { if (!strnicmp(s+1,exc_str,len)) return NOT_FOUND;
      }
      else
      { if (!strncmp(s+1,exc_str,len)) return NOT_FOUND;
      }
    else
    { Char tuch = 0;
      if (exc_opt & 4) tuch = exc_str[0];
      for (; *++s != 0; )
        if (*s == exc_str[0] || toupper(*s) == tuch)
          if (exc_opt & 4)
          { if (!strnicmp(s,exc_str,len)) return NOT_FOUND;
          }
          else
          { if (!strncmp(s,exc_str,len)) return NOT_FOUND;
          }
    }
  }

  props &= M_MATCHWORD+M_IC;

{ Char pat0 = (props & M_IC) == 0 ? *pat : toupper(*pat);
  Bool unkn = pat0 == '[' or pat0 == '.';
  Byte mw = (props & M_MATCHWORD);
  
  if (mw == 0 and not anchored)
    for ( ;*line != 0; ++line)
    { if (unkn or (not props ? *line == pat0 : toupper(*line) == pat0))
      { if (match_re(line, pat, props) > line)
	  return OK;
      }
    }
  else
#define inw(ch) (isalpha(ch) or isdigit(ch) or ch == '_' or ch == '$')
  { Bool inword = false;
    props &= M_IC;
    for ( ;*line != 0; ++line)
    { if (not inword)
      { if (unkn or (not props ? *line == pat0 : toupper(*line) == pat0))
	{ Char * ln = match_re(line, pat, props);
	  if (ln > line and (not mw or not inw(*ln)))
	    return OK;
	}
        if (anchored)
	  break;
      }
      inword = inw(*line);
    }
  }

  return NOT_FOUND;
}}
