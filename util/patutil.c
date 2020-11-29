#include   <stdio.h>
#include   <ctype.h>
#include   <sys/types.h>
#include   "build.h"
#include   "strutil.h"



/*
  CMDMATCH: if t_ is a prefix of s_ and 
	    s__ is the following string in s_ 
	    then if s__[0] is not a word continuing character
	    return s__ 
	    otherwise return null
 */
public Char * cmdmatch(t_, s_)
	Char *t_, *s_;
{ fast Char * t = t_;
  fast Char * s = s_;

#if CHARCODE != ASCII
  for (; *s != 0 and *t != 0 and toupper(*t) == toupper(*s); ++s)
    ++t;
#else
  for (; *s != 0 and *t != 0 and (*t | 0x20) == (*s | 0x20); ++s)
    ++t;
#endif
  return *t != 0 or isalnum(*s) or *s == '_' or *s == '$' ? null : s;
}




			/* Warning: This code is non reentrant if
				    wspace is null
			 */
static Byte mysrchset[256];

/*
  STRSRCHPAT:  search s_ for the first occurrence of a character in
	       wspace if wspace is non null otherwise in the string set
	       wspace is a mapping from 0-255 to Bool
	       set is a string of the set characters
 */
public Char * pat_strsrchset(wspace_, s_, set)
	 Byte *  wspace_;
	 Char *  s_;
	 Char *  set;
{ fast Char * s;
       Byte * wspace = wspace_;
  
  if (wspace == null)			/* build wspace from set */
  { wspace = &mysrchset[0];
    
    for (s = &set[-1]; *++s != 0; )
      wspace[*s] = 1;
  }
      
  for (s = &s_[-1]; *++s != 0; )
    if (wspace[*s] != 0)
      break;

{ Char * res = *s == 0 ? null : s;

  if (wspace_ == null)			/* restore mysrchset */
    for (s = &set[-1]; *++s != 0; )
      mysrchset[*s] = 0;

  return res;
}}


		
/*
  STR_DFLTSRCHSET:   load a search set into wspace
 */
public void pat_dfltsrchset(wspace, set)
	Byte *   wspace;
	Char *   set;
{ fast Char * s;
  fast Byte * ws = wspace != null ? wspace : &mysrchset[0];

  for (s = &set[-1]; *++s != 0; )
    ws[*s] = 1;
}


/*
  STR_UNDFLTSET:   unload a search set from wspace
 */
public void pat_undfltsrchset(wspace, set)
	Byte *   wspace;
	Char *   set;
{ fast Char * s;
  fast Byte * ws = wspace != null ? wspace : &mysrchset[0];

  for (s = &set[-1]; *++s != 0; )
    ws[*s] = 0;
}


