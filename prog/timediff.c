#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "build.h"
#include "../common/h/msdir.h"

extern double atof(const char *);


#define MAXLN 1024

static char ipline[MAXLN+2];
static char opfname[MAXLN+2];

private void explain()

{ fprintf(stderr, "timediff {-#} filename*\n   -#   deduct mins for lunch\n"
	 	  " Recognises lines of form {h}h:mm to hh:mm\n"
	 );

}


Bool is_diry(fd)

{	struct stat fstat_;
	int cc = fstat(fd, &fstat_);
	return cc == OK and (fstat_.st_mode & 040000) != 0;
}

#if 0
#define Sint Int
#else
#define Sint
 Int stt_hr;
 Int stt_min;
 Int mins;
 Int hrs;

#endif

  Int tot_hrs = 0;
  Int tot_mins = 0;
  Int lunch = 0;

void doit(FILE * chan)

{   Int ltot_hrs = 0;
    Int ltot_mins = 0;
    
    ipline[0] = ' ';

    while (true)
    { Char * ln = fgets(&ipline[1], MAXLN, chan);
      if (ln == null)
	break;
      ln[strlen(ln)-1] = 0;
      
      fputs(ln, stdout);
    { Sint stt_hr  = 0;
      Sint stt_min = 0;
      
      for (;*++ln != 0; )
	if (ln[0] == ':' and
	    in_range(ln[-1], '0', '9') and
	    in_range(ln[1], '0', '9') and
	    in_range(ln[2], '0', '9') )
	{ Int hr  = atol(&ln[-2]);
	  Int min = atol(&ln[1]);
          if (stt_hr == 0)
          { stt_hr = hr;
            stt_min = min;
          }
          else
	  { float ded_float = atof(&ln[3]) * 60.0;
	    Int ded = ded_float;
	    if (ln[3] == 0)
	      ded = lunch;
	  { Sint mins = (hr - stt_hr) * 60 + min - stt_min;
	    Sint hrs = mins / 60;
	    mins -= hrs * 60;
	    mins -= ded;
	    while (mins < 0)
	    { hrs -= 1;
	      mins += 60;
	    }
	    ltot_hrs += hrs;
	    ltot_mins += mins;
	    printf("\t%2ld.%ld", hrs, mins * 100 / 60);
	    if (ded > 0)
	      printf("\tnet");
	    break;
	  }}
	}
      
      fputs("\n", stdout);
    }}

    while (ltot_mins >= 60)
    { ltot_mins -= 60;
      ltot_hrs += 1;
    }
    
    tot_hrs += ltot_hrs;
    tot_mins += ltot_mins;

    while (tot_mins >= 60)
    { tot_mins -= 60;
      tot_hrs += 1;
    }
    
    printf("				   %ld.%ld\t%ld.%ld\n", 
		    ltot_hrs, ltot_mins * 100 / 60,
		    tot_hrs, tot_mins * 100 / 60);
}




void main(argc, argv)
        short   argc;
        Char ** argv;
{ if (argc <= 1)
  { explain();
    exit(0);
  }
  
{ Bool gotstdin = false;
  Bool gotfn = false;
  
  Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];
  
  for (; argsleft > 0; --argsleft)
  { if (argv_[0][0] == '-')
    { if (argv_[0][1] == 0)
        gotstdin = true;
      else
      { if (in_range(argv_[0][1], '0', '9'))
        { lunch = atol(&argv_[0][1]);
          fprintf(stderr, "Lunch mins %d\n", lunch);
        } 
        else
        { explain();
          exit(0);
        }
      }
    }
    else
    { Char * eny;

      if (gotstdin)
        doit(stdin);

      msd_init(argv_[0], null, MSD_REPEAT | MSD_NOCHD);

      while ((eny = msd_nfile()) != null)
      { printf("FILE %s\n", eny);
      { FILE * f = fopen(eny, "r");
	if (f == null)
	  fprintf(stderr, "Cannot open %s\n", eny);
	else
	{ doit(f);
	  fclose(f);
	}
      }}
      gotfn = true;
    }

    ++argv_;
  }
  if (not gotfn)
    doit(stdin);
}}
