#include <stdio.h>
#include "build.h"


private Int  lineix = 0;
private Int  linect = 0;
private Int tabsz0 = 8;
private Int tabsz = 8;
private Int pagelen = 66;


#define VT 12



static void explain()
      { fprintf(stderr, "untab {-W #} {-w #} {-l #} { file }\n"
                        "     W -- first tab width\n"
                        "     w  -- tab width\n"
                        "     l -- pagelen\n"
                              
      	       );
        exit(0);
      }


void main (argc,argv)
	Vint argc;
        Char ** argv;
{ Vint argsleft = argc - 1;
  Char ** argv_ = &argv[1];

  for (; argsleft > 0 and argv_[0][0] == '-'; --argsleft)
  { Char * flgs;
    for (flgs = &argv_[0][1]; *flgs != 0; ++flgs)
      if      (*flgs == 'w')
      { if (in_range(flgs[1], '0', '9'))
	  tabsz = atoi(&flgs[1]);
	else if (argsleft > 0)
	{ --argsleft;
	  if (argsleft <= 0)
	    explain();

	  tabsz = atoi(*++argv_);
	}
      }
      else if (*flgs == 'W')
      { if (in_range(flgs[1], '0', '9'))
	  tabsz0 = atoi(&flgs[1]);
	else if (argsleft > 0)
	{ --argsleft;
	  if (argsleft <= 0)
	    explain();

	  tabsz0 = atoi(*++argv_);
	}
      }
      else if (*flgs == 'l')
      { if (in_range(flgs[1], '0', '9'))
	  pagelen = atoi(&flgs[1]);
	else if (argsleft > 0)
	{ --argsleft;
	  if (argsleft <= 0)
	    explain();
	  pagelen = atoi(*++argv_);
	}
      }
      else
        explain();
    ++argv_;
  }
  lineix = 0;
  linect = 0;
  
{ Char * srcfn = argsleft <= 0 ? null : argv_[0];
  FILE * file = srcfn == null ? stdin : fopen(srcfn, "r");
 
  if (file == null)
    fprintf(stderr,srcfn == null ? "Cannot open stdin\n"
    				 : "Cannot open file %s\n", srcfn);
  else 
    while (true)
    { Short ch = getc(file);
      ++lineix; 
      
      if (ch <= EOF)
        break;
                
      if (ch == '\n')
      { lineix = 0;
	linect += 1;
      }
      if      (ch == '\t')
      { putchar(' ');
        if (lineix < tabsz0)
          for (; lineix < tabsz0; ++lineix)
            putchar(' ');
        else
          for (; lineix % tabsz != 0; ++lineix)
            putchar(' ');
      }
      else if (ch == VT)
      { while (++linect <= pagelen)
	  putchar('\n');
	linect = 0;
      }                   	   
      else 
        putchar(ch);
    }  
}}   
