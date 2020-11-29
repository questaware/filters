#include  <stdio.h>
#include  <ctype.h>
#include  "build.h"
#include  "../h/defs.h"
#include "../common/h/eprintf.h"

/* leap_year(y) == y mod 400 == 0 or 
                   y mod 100 != 0 and y mod 4 == 0
*/
extern long time();
#define UNIX_TO_BASE (366+365+365)
#define BASE_YEAR 1973
#define F1461  1461
#define F1095  1095
#define F365   365
#define F10227 ((F365*4 + 1)*7)

			/* first entry not used */
private Int M0 [] = {0,0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
private Int M1 [] = {0,0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

private Int W [] = { 52,52,52,53,52,52,52,
		     52,53,52,52,52,52,52,
		     53,52,52,52,52,53,52,
		     52,52,52,52,53,52,52};

private Int offset [] = {24, 13, 2, 21, 10, 28, 18, 7, 26, 15, 
                          4, 23, 12, 1, 20, 9, 27, 17, 6};
typedef struct 
{ Short y, m, d;
} Dmy_t, *Dmy;


typedef struct 
{ Short  oldstate;
  Char ch;
  Short  newstate;
} Fse;

Fse fsa [] = {{0, 'J', 1}, {0, 'F', 2}, {0, 'M', 3}, {0, 'A', 4}, {0, 'S', 5},
              {0, 'O', 6}, {0, 'N', 7}, {0, 'D', 8},
	      {1, 'A', 9}, {1, 'U',10},
	      {9, 'N',-1}, {10,'N',-6}, {10,'L',-7}, 
	      {2, 'E',11}, {11,'B',-2}, 
	      {3, 'A',12}, {12,'R',-3}, {12,'Y',-5},
	      {4, 'P',13}, {4, 'U',14}, {13,'R',-4}, {14, 'G', -8},
	      {5, 'E',15}, {15,'P',-9},
	      {6, 'C',16}, {16,'T',-10},
	      {7, 'O',17}, {17,'V',-11},
	      {8, 'E',18}, {18,'C',-12}, {-1,'0',0}};

static Char * MONTH [] = { "???", "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

public Date day_no(y, m, d)
	Short y, m, d;
{ if ((unsigned)m > 12)
    return 0;
  if ((unsigned)d > 31) 
    return 0;

  return ((y - BASE_YEAR) >> 2) * F1461 + d + 
                    ((y & 3) == 0 ? F1095 + M1[m] : ((y-1) & 3) * F365 + M0[m]);
}


    
    
private void to_date(dn, dmy_ref)
	Short    dn;
	Dmy_t *  dmy_ref;
{ if ((dn % F1461) == 0)
  { dmy_ref->y = BASE_YEAR + (dn / F1461) * 4 - 1;
    dmy_ref->m = 12;
    dmy_ref->d = 31;
  }
  else 
  { Int dd = dn % F1461; 
    dmy_ref->y = BASE_YEAR + (dn / F1461) * 4 + (dd - 1) / F365;
    
  { Int dd_ = ((dd - 1) % F365) + 1;
    Int m = 12;
    
    if ((dmy_ref->y & 3) == 0)
    { while (dd_ <= M1[m]) 
        --m;
      dmy_ref->d = dd_ - M1[m];
    }
    else 
    { while (dd_ <= M0[m]) 
        --m;
      dmy_ref->d = dd_ - M0[m];
    }
    dmy_ref->m = m;
  }}
}  
    
public Short to_week(dn) 
        Date dn;
{ Short dd = dn % F10227;
  Short yr;
  Short wk;
 
  if (dd == 0)
  { yr = 0;
    wk = 52;
  }
  else 
  { yr = 1;
    wk = (dd - 1)/7 + 1;
    while (wk > W[yr])
    { wk -= W[yr];
      yr += 1;
    }
  }
/*yr += BASE_YEAR + dn/F10227)*28 - 1; */
  return wk;
}


  
public Date easter(y)
	Short y;
{ Short g = 1 + y % 19;
  return ((offset[g] +  day_no(y,3,21))/7 + 1)*7;
}

/* shrove tuesday = easter - 40
   ascension day  = easter + 39
   whit monday    = easter + 50
*/



public Short next_month(d)
        Short d;
{ Dmy_t  dmy;  to_date(d, &dmy);
  dmy.d = 0;
  dmy.m += 1;
  if (dmy.m == 13)
  { dmy.y += 1;
    dmy.m = 1;
  }
  return day_no(dmy.y, dmy.m, dmy.d);
}

public Date asc_to_date(str)
	Char * str;
{ fast Char * s = &str[0];
       Short day = 0;

  while (isspace(*s))
    ++s;
    
  if (*s == 0)
    return time(0) / (60*60*24) - UNIX_TO_BASE + 1;

  for (; isdigit(*s); ++s)
    day = day * 10 + *s - '0';
    
  if (not in_range(day, 1, 31))
    return -1;

  while (isspace(*s))
    ++s;
    
{ Fse * p = &fsa[-1];
  Short cstate;
  for (cstate = 0; cstate >= 0 ; cstate = p->newstate)
  {  
    while ((++p)->oldstate != cstate)
      if (p->oldstate == -1)
        return -2;
    while (p->oldstate == cstate and p->ch != toupper(*s))
      ++p;
    if (p->oldstate != cstate)
      return -3;
    
    ++s;
  }

  while (isspace(*s))
    ++s;
{ Int yr = -1;
  sscanf(s, "%d", &yr);
  if (in_range(yr, 70, 99))
    yr += 1900;
  if (not in_range(yr, BASE_YEAR, 2054))
    return -4;
   
{ Date dn = day_no((Short)yr, -cstate, day);
  Dmy_t dmy; to_date(dn, &dmy);
  
/*  printf("YR %d, MTH %d, DAY %d\n", dmy.y, dmy.m, dmy.d);*/
  if (dmy.y != yr or dmy.m != -cstate or dmy.d != day)
    return -5;
  return dn;
}}}}          

public Char * date_to_asc(str, dn)
	Date dn;
	Char * str;
{ *str = 0;
  if (dn >= 0)
  { Dmy_t dm;  to_date(dn, &dm);
    if (dm.y >= 1900)
      dm.y -= 1900;
    eprintf(str, dm.d < 10 ? " %d %s %d"
                           : "%d %s %d", (Short)dm.d, MONTH[dm.m], (Short)dm.y);
  }
  return str;
}



public Int date_to_day(dn)
	Date dn;
{ return dn % 7;
}
