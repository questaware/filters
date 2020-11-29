#include <string.h>
#include <stdlib.h>

#include	"build.h"
#include	"time.h"
#include	"ctype.h"


#if S_MSDOS
#define BASE_YEAR 1980
#else
#define BASE_YEAR 1900
#endif

static Char months[] = "JAN.FEB.MAR.APR.MAY.JUN.JLY.AUG.SEP.OCT.NOV.DEC";
			  
#define M_SEC   1
#define M_MIN   2
#define M_HOUR	4
#define M_DAY   8
#define M_MTH  16
#define M_YEAR 32


Time  asc_to_tad(const Char *   s)

{ struct tm time_dcr;
  Vint osep = 0;
  Vint state = 0;
  memset(&time_dcr, 0, sizeof(struct tm));
#if 0
  fputs("STT", stderr);
#endif

  for ( ; ; ++s)
  { while (*s != 0 and isspace(*s))
      ++s;
    if (*s == 0)
      break;
  { register Vint ix;
    if      (isalpha(*s))
    { for (ix = 48; (ix-=4) >= 0; )
        if (toupper(s[0]) == months[ix  ] and
	    toupper(s[1]) == months[ix+1] and
	    toupper(s[2]) == months[ix+2])
	{ state |= M_MTH;
	  time_dcr.tm_mon = ix >> 2;
	  break;
	}
      while (isalpha(*s))
        ++s;
    }
    else if (isdigit(s[0]))
    { int v = atoi(s);
      
      while (isdigit(*s))
        ++s;
      
      if (state == 0 && 
           (s[0] == '/' || s[0] == 0 || s[0] == ':') &&
            in_range(v, 19000000, 21000000))
      { int m = v;
        state |= M_YEAR + M_MTH + M_DAY;
        v /= 10000;
        time_dcr.tm_year = v-BASE_YEAR;
        m = (m - v * 10000);
	time_dcr.tm_mon = m / 100 - 1;
	time_dcr.tm_mday = m - (time_dcr.tm_mon + 1)*100;
      }
      else if      (s[0]== '/' or s[0] == 0 and osep == '/')
      { if (state == 0)
        { state |= M_YEAR;
          if (in_range(v, 60, 99))
            v += 1900;
          else if (in_range(v, 0, 20))
            v += 2000;
          time_dcr.tm_year = v-BASE_YEAR;
        }
	else if ((state & M_MTH) == 0)
	{ state |= M_MTH;
	  time_dcr.tm_mon = v-1;
	}
        else if	((state & M_DAY) == 0)
	{ state |= M_DAY;
	  time_dcr.tm_mday = v;
	}
      }
      else if (s[0] == ':')
      { if	((state & M_DAY) == 0)
	{ state |= M_DAY;
	  time_dcr.tm_mday = v;
	}
        else if	((state & M_HOUR) == 0)
	{ state |= M_HOUR;
	  time_dcr.tm_hour = v;
	}
	else if ((state & M_MIN) == 0)
	{ state |= M_MIN;
	  time_dcr.tm_min = v;
	}
	else
        { state |= M_SEC;
          time_dcr.tm_sec = v;
        }
      }
    }
    osep = s[0];
  }}
#if 0
  eprintf(null, "SSSSSSSS %d %d %d %d %d %d\n", time_dcr.tm_year,
					    time_dcr.tm_mon,
					    time_dcr.tm_mday,
					    time_dcr.tm_hour,
					    time_dcr.tm_min,
					    time_dcr.tm_sec);
#endif
#if S_MSDOS
{ int dst = 0;
  if (in_range(time_dcr.tm_mon, 5, 8))
    dst = 1;

  return ((Int)((time_dcr.tm_year << 9) +
	        ((time_dcr.tm_mon+1) << 5) +
	         time_dcr.tm_mday) << 16) +
	   ((time_dcr.tm_hour - dst)<< 11) +  /* was +? */
	   (time_dcr.tm_min  << 5) +
	    (time_dcr.tm_sec >> 1);
}
#else
  return mktime(&time_dcr);
#endif
}

struct tm time_dcr;

Char * tad_to_asc(Nat4 secs)  /* not secs in S_WIN32 ! */

{ 
#if S_MSDOS
  static char res[24];
  
/*eprintf(null, " (%lx %d) ", secs, (secs >> 11) & 0x1f);*/

  time_dcr.tm_sec = (secs & 0x1f) * 2;
  secs >>= 5;
  time_dcr.tm_min = secs & 0x3f;
  secs >>= 6;
  time_dcr.tm_hour = secs & 0x1f;
  secs >>= 5;
  time_dcr.tm_mday = secs & 0x1f;
  secs >>= 5;
  time_dcr.tm_mon = (secs & 0xf) - 1;
  secs >>= 4;
  time_dcr.tm_year = (secs & 0x7f) + 80;
  if (in_range(time_dcr.tm_mon, 5, 8))
    time_dcr.tm_hour += 1;

#if S_WIN32
  if (time_dcr.tm_year >= 100)
     time_dcr.tm_year -= 100;
#else
  if (time_dcr.tm_year >= 180)		/* why, I dont know */
     time_dcr.tm_year -= 100;
#endif
#else
  struct tm * res = gmtime(&secs);
  if (res != 0)
    memcpy(&time_dcr, res, sizeof (struct tm));
#endif
  return asctime(&time_dcr);
}
