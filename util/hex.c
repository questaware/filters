#include   <stdio.h>
#include   <ctype.h>
#include   "build.h"
#include   "hex.h"

#define chr_in_set(ch,set) (set[ch])


static char to_hexdig[] = "0123456789abcdef";
static char to_uhexdig[] = "0123456789ABCDEF";

public Char * memtohex(t_, s_, n)
	Char    * t_;
	Char    * s_;
	Short     n;
{ fast Char * t = t_;
  fast Char * s = s_;
  
  while (--n >= 0)
  { char dig = 
    *t++ = to_hexdig[(*s >> 4) & 0xf];
    *t++ = to_hexdig[*s & 0xf];
    ++s;
  }
  *t = 0;
  return t;  
}	



public Int hextomem(t_, s_, n_)
	Char * t_, * s_;
	Short  n_;		/* in nibbles */
{      Int n = n_;
  fast Char * t = &t_[-1];
  fast Char * s = s_;
       Bool top = true;

  for ( ; --n >= 0; ++s)
  { Char d = toupper(*s) - '0';
    if      (in_range(d, 0, 9))
      ;						/* d is OK */
    else if (in_range(d, 'A'-'0', 'F'-'0'))
      d -= 'A' + 10;				/* adjust d */
    else
      break;					/* illegal character */

    if (top)
    { ++t;					/* left nibble */
      *t = d << 4;
      top = false;
    }
    else
    { *t |= d;					/* right nibble */
      top = true;
    }
  }

  return n_ - n - 1;
}

static Char ghset[256] = {0};		/* top bit: set of legal chars in hex */
					/* rest:    hex value of the char     */

/*
  GETHEXSTR:  extracts a hex value from s_
	      returns s_ if no hex value was found 
	      otherwise a pointer to the following characters
	      the result is in t_
 */
public Char * gethexstr(t_, s_, n)
	Byte *  t_;
	Char *  s_;
	Short   n;			/* -ve => accept only definate hex */
{ fast Byte * t = t_;
  fast Char * s = s_;
       Bool gottop = false;
       Bool gothex = n > 0;
       
  if (not gothex)
    n = -n;					/* make n non negative */

  if (ghset['0'] == 0)				/* it's not initialised */
  { Int i = 0x80;
    for (s = &to_uhexdig[-1]; *++s != 0; ++i)
      ghset[*s] = i;
    
    i = 0x80;
    for (s = &to_hexdig[-1]; *++s != 0; ++i)
      ghset[*s] = i;
  }
  
  if (s[0] == '0' and toupper(s[1]) == 'X')
  { gothex = true;				/* prefix is '0X' */
    s += 2;
  }
    
  for (; --n >= 0 and chr_in_set(*s, ghset); ++s)
  { if (*s > '9')		/* ASCII */
      gothex = true;				/* it's definately hex */
    if (not gottop)
      *t = ghset[*s] << 4;			/* first nibble */
    else
      *t++ |= ghset[*s] & 0xf;			/* second nibble */
  }

  return not gothex ? s_ : s;
}

/*
  GETHEX:   extracts a hex value from s_
	    returns s_ if no hex value was found 
	    otherwise a pointer to the following characters
 */
public Char * gethex(int_ref, s_, n)
	Int    *int_ref;
	Char *  s_;
	Short   n;			/* -ve => accept only definate hex */
{      Int    res;
  fast Char * s = s_;
       Bool   gothex = n > 0;
       
  if (not gothex)
    n = -n;					/* make n non negative */

  if (ghset['0'] == 0)				/* not initialised */
  { Int i = 0x80;
    for (s = &to_uhexdig[-1]; *++s != 0; ++i)
      ghset[*s] = i;
    
    i = 0x80;
    for (s = &to_hexdig[-1]; *++s != 0; ++i)
      ghset[*s] = i;
  }
  
  if (s[0] == '0' and toupper(s[1]) == 'X')
  { gothex = true;				/* prefix was '0X' */
    s += 2;
  }
    
  for (; --n >= 0 and chr_in_set(*s, ghset); ++s)
  { if (*s > '9')		/* ASCII */
      gothex = true;				/* it's definately hex */
    res = (res << 4) | (ghset[*s] & 0xf);
  }

  if (not gothex)
    return s_;					/* failed */
  else
  { *int_ref = res;				/* succeeded */
    return s;
  }
}
