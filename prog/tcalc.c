#include <stdio.h>
#include <time.h>
#include "build.h"




static void explain()
{ fputs("TZ=v tcalc {-z} {-s format} {-t format} {{+|-}{YMDhms} #}* {date}\n"
        "  -z             -- return code 1 => DST\n"
	"  -s format      -- source format subset of strftime\n"
	"                 -- DEFAULT%Y/%m/%d %H:%M:%S\n"
        "  -t format      -- target format as per strftime\n"
        " {+|-}{YMDhms} # -- add/subtract # years/months/day/h/m/s\n"
	"For both formats additionally: %N means seconds since Jan 1970\n"
	"A return code of not 0 (-z: 2) indicates failure\n", stderr);
   exit(2);
}



Cc sscanft(struct tm * tm_p, time_t * time_p, char * fmt, char * src)

{ *time_p = -1;
  memset(tm_p, 0, sizeof(*tm_p));
  while (*src != 0)
  { while (*src == *fmt && *fmt != '%' && *fmt != 0)
    { ++src;
      ++fmt;
    }
    if (*src == 0 || *fmt == 0)
      break;
    if (*fmt != '%' || fmt[1] == 0)
    { if (*src != *fmt)
        break;
    }  
    else
    { switch (fmt[1])
      { case 'y': tm_p->tm_year = (src[0]-'0')*10+(src[1]-'0');
                  src += 2;
        when 'Y': tm_p->tm_year = (((src[0]-'0')*10+(src[1]-'0'))*
					10+(src[2]-'0'))*10+(src[3]-'0')-1900;
		  src += 4;
        when 'm': tm_p->tm_mon = (src[0]-'0')*10+(src[1]-'0') - 1;
		  src += 2;
        when 'd':
        case 'e': tm_p->tm_mday = (src[0]-'0')*10+(src[1]-'0') - 1;
		  src += 2;
        when 'H': tm_p->tm_hour = (src[0]-'0')*10+(src[1]-'0') - 1;
		  src += 2;
        when 'M': tm_p->tm_min  = (src[0]-'0')*10+(src[1]-'0') - 1;
		  src += 2;
        when 'S': tm_p->tm_min  = (src[0]-'0')*10+(src[1]-'0') - 1;
		  src += 2;
        when 'N': if (in_range(src[0], '0', '9'))
		    *time_p = strtol(src, &src, 10);
      }
      fmt += 2;
    }
  }

  return *fmt == 0 ? OK : -1;
}





void main (argc,argv)
	Vint argc;
        Char ** argv;
{ Vint argsleft = argc - 1;
  Char ** argv_ = &argv[1];
  struct tm tm;

  Char * sfmt = "%Y/%m/%d %H:%M:%S";
  Char * tfmt = null;

  static int dayadd[2];
  static int monadd[2];
  static int yradd[2];
  static int houradd[2];
  static int minadd[2];
  static int secadd[2];
  
  char res[1000];
  time_t then;
  Bool fromthen = false;
  int z_opt = 0;

  for (; argsleft > 0 and (argv_[0][0] == '-' || argv_[0][0] == '+'); --argsleft)
  { Char * flgs;
    for (flgs = &argv_[0][1]; *flgs != 0; ++flgs)
      if      (*flgs == 's' && argsleft > 1 && argv_[0][0] == '-')
      { --argsleft;
	++argv_;
        sfmt = argv_[0];
      }
      else if (*flgs == 't' && argsleft > 1 && argv_[0][0] == '-')
      { --argsleft;
	++argv_;
        tfmt = argv_[0];
      }
      else if (*flgs == 'D' && argsleft > 1)
      { --argsleft;
	++argv_;
        dayadd[argv_[-1][0] == '+'] = atoi(argv_[0]);
      }
      else if (*flgs == 'M' && argsleft > 1)
      { --argsleft;
	++argv_;
        monadd[argv_[-1][0] == '+'] = atoi(argv_[0]);
      }
      else if (*flgs == 'Y' && argsleft > 1)
      { --argsleft;
	++argv_;
        yradd[argv_[-1][0] == '+'] = atoi(argv_[0]);
      }
      else if (*flgs == 'h' && argsleft > 1)
      { --argsleft;
	++argv_;
        houradd[argv_[-1][0] == '+'] = atoi(argv_[0]);
      }
      else if (*flgs == 'm' && argsleft > 1)
      { --argsleft;
	++argv_;
        minadd[argv_[-1][0] == '+'] = atoi(argv_[0]);
      }
      else if (*flgs == 's' && argsleft > 1)
      { --argsleft;
	++argv_;
        secadd[argv_[-1][0] == '+'] = atoi(argv_[0]);
      }
      else if (*flgs == 'z')
        z_opt = 1;
      else
        explain();
    ++argv_;
  }

  secadd[0] += minadd[0] * 60 + houradd[0] * 3600;
  secadd[1] += minadd[1] * 60 + houradd[1] * 3600;


  if (argsleft > 0)
  { time_t tim;
    Cc cc = sscanft(&tm, &tim, sfmt, argv_[0]);
    if (cc != OK)
    { fprintf(stderr, "Not parsed (%d)\n", cc);
      exit(2);
    }

    if ((dayadd[0] | dayadd[1]) != 0 ||
        (secadd[0] | secadd[1]) != 0 || tim != -1)
    { then = tim;
      if (then == -1)
      { tm.tm_isdst = -1;
        then = mktime(&tm);
      }
      if (then == -1)
      { fprintf(stderr, "Not Converted\n");
        exit(2);
      }

      then += (dayadd[1] - dayadd[0]) * 86400 + secadd[1] - secadd[0];

      fromthen = true;
    }
  }
  else
  { then = time(null) + (dayadd[1] - dayadd[0]) * 86400 + secadd[1] - secadd[0];
    fromthen = true;
  }

  if (fromthen)
  { tm.tm_isdst = 0;

  { struct tm * tm_p = localtime(&then);
    if (tm_p == null)
    { fprintf(stderr, "Internal Error\n");
      exit(2);
    }
    memcpy(&tm, tm_p, sizeof(struct tm));
  }}

  tm.tm_year -= yradd[0];
  tm.tm_year += yradd[1];
  tm.tm_mon -= monadd[0];
  tm.tm_mon += monadd[1];

  while (tm.tm_mon < 0)
  { 
    tm.tm_year -= 1;
    tm.tm_mon += 12;
  }
  while (tm.tm_mon > 11)
  { 
    tm.tm_year += 1;
    tm.tm_mon -= 12;
  }

  if (tfmt != null && strlen(tfmt) + 10 >= sizeof(res))
  { fprintf(stderr, "Result is too long (MAX 990 bytes)\n");
    exit(2);
  }
  
{ char * t;
  if (tfmt != null)
    for (t = tfmt; *t != 0; ++t)
      if (*t == '%' && t[1] == 'N')
      { t[1] = 'd';
        break;
      }
  
  if (tfmt != null && *t != 0)
  { printf(tfmt, mktime(&tm));
    printf("\n");
  }
  else
  { if (tfmt == null) tfmt = "%Y/%m/%d %H:%M:%S";

  { int len = strftime(&res[0], sizeof(res)-1, tfmt, &tm);
    if (len > 0)
      printf("%s\n", res);
    else
      exit(2);
  }}
  exit(z_opt * (tm.tm_isdst == 1));
}}

