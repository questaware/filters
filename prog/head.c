#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "version.h"



#define MAXLN 1024

static char ipline[MAXLN+2];


private void explain()

{ fprintf(stderr, "head -# filename*\n");
  fprintf(stderr, "   -# -- Number of lines\n");
}

void main(argc, argv)
        Int     argc;
        Char ** argv;
{ if (argc <= 1)
  { explain();
    exit(0);
  }
  
{ Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];
  
  Bool line_ct = 10;

  for (; argsleft > 0; --argsleft)
  { if (argv_[0][0] == '-')
    { if (in_range(     argv_[0][1], '0', '9'))
        line_ct = atol(&argv_[0][1]);
      else
      { explain();
        exit(0);
      }
    }
    else
    { Char * fn = argv_[0];
      FILE * chan = fopen(fn , "r");
      if (chan == null)
        fprintf(stderr, "Cannot open %s\n", fn);
      else
      { while (--line_ct >= 0)
	{ Char * ln = fgets(&ipline[0], MAXLN, chan);
	  if (ln == null)
	    break;
	  fputs(ln, stdout);
	}      
        fclose(chan);
      }
    }

    ++argv_;
  }

}}
