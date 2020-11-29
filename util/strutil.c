#include   <stdio.h>
#include   <ctype.h>
#include   <sys/types.h>
#include   "build.h"
#define C_STRUTIL
#include   "strutil.h"

/*
  STRMATCH:   return a pointer into t_
	      the result is the first character not matching s or
	      the terminator character
 */
 Char * strmatch(Char *t_, Char *s_)

{ fast Char * t = t_;
  fast Char * s = s_;

#if CHARCODE != ASCII
  for (; *s != 0 and *t != 0 and toupper(*t) == toupper(*s); ++s)
    ++t;
#else
  for (; *s != 0 and *t != 0 and (*t | 0x20) == (*s | 0x20); ++s)
    ++t;
#endif
  return t;
}



#if 0

/*
  MEM_TO_PRTABLE: Convert a string to (an ambiguious) printable representation.
 */
 Char * mem_to_prtable(Char   * t_, Char *   s_, Short    n)

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

#endif


/*
  STRUCCMP:   like strcmp but ignores charcter case
              Result: the difference of the first different character pair 
 */
 Short struccmp(const Char *t_, const Char *s_)

{  const Char * t = t_;
   const Char * s = s_;

  for (; *s != 0 && *t != 0 && toupper(*t) == toupper(*s); ++s)
    ++t;
  return (unsigned)*t - (unsigned)*s;
}

/*
  STRUCCMP:   like strcmp but ignores charcter case
              Result: the difference of the first different character pair 
 */
 Short strnicmp(const Char *t_, const Char *s_, int n)

{  const Char * t = t_;
   const Char * s = s_;

  for (; --n >= 0 && *s != 0 && *t != 0 && toupper(*t) == toupper(*s); ++s)
    ++t;
  return n == 0 ? 0 : (unsigned)*t - (unsigned)*s;
}



 Char * skipspaces(Char *s_)

{  Char * s = s_;

  for (; *s != 0 and isspace(*s); ++s)
    ;
  return s;
}


#if 0

 Char * skipalnum(s_)
	Char *s_;
{  Char * s = s_;

  for (; *s != 0 and isalnum(*s); ++s)
    ;
  return s;
}



/*
  GETALNUM:   Copy at most n of the leading alphanumerics of s_ to t_
              Result: Pointer to the first character in s_ not alphanumeric
 */
 Char * getalnum(t_, s_, n)
	Char *  t_;
	Char *  s_;
	Short   n;
{  Char * t = t_;
   Char * s = s_;

  for (; --n >= 0 and *s != 0 and isalnum(*s); ++s)
    *t++ = *s;
  return s;
}



#endif

/*
  STRPCPY:  Copy at most n-1 characters from s_ to t_;
	    0 is the end of string character;
	    the last character stored is always 0
 */
#if S_HPUX
char * strpcpy(t_, s_, n_)
	Char * t_, *s_;
	Short  n_;
#else
char * strpcpy(Char * t_, Char * s_, Short n_)

#endif
{  char * t = t_;
   char * s = s_;
   short n = n_;
  
  while (--n > 0 && (*t++ = *s++) != 0)
    ;
  
  if (n == 0)
    *t = 0;
  return t_;
}



/*
  STRPCAT:  Copy at most n-1 characters from s_ to t_;
	    0 is the end of string character;
	    the last character stored is always 0
 */
#if S_HPUX
char * strpcat(t_, s_, n_)
	Char * t_, *s_;
	Short  n_;
#else
char * strpcat(Char * t_, Char * s_, Short n_)

#endif
{  char * t = t_;
   char * s = s_;
   short n = n_;
  
  while (*t++ != 0)
    --n;
    
  --t;
  
  while (--n > 0 && (*t++ = *s++) != 0)
    ;
  
  if (n == 0)
    *t = 0;
  return t_;
}

#if 0

/*
  STRCHCPY:   Replicate ch n times in t_; follow them by a 0
 */
 Char * strchcpy(t_, ch_, n)
        Char * t_;
        Char   ch_;
        Short  n;
{  Char * t = t_;
   Char  ch = ch_;
  
  while (--n >= 0)
    *t++ = ch;
  *t = 0;
  return t_;
}



/*
  STRWCPY: Copy the leading non space characters of s_ to t_
	   where non space means code > ' '
 */
 Char * strwcpy(t_, s_)
        Char * t_, * s_;
{  Char * t = t_;
   Char * s = s_;
 
 while (*s > ' ')
   *t++ = *s++;
   
 *t = 0;
 return t_; 
}

#endif
