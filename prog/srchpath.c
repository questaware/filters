#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "version.h"
#include "../../util/h/strmpath.h"


extern Char * getenv();



private void explain()

{ fprintf(stderr, "srchpath [-ax/] filename path\n");
  fprintf(stderr, "   -a -- Print all findings\n");
  fprintf(stderr, "   -d -- Print only directory part of names\n");
  fprintf(stderr, "   -x -- Allow only executable files\n");
  fprintf(stderr, "   -/ -- Print out as absolute filenames\n");
  fprintf(stderr, "  where path is a colon separated list of directories\n");
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

  Set opts = 0;
  Bool u_opt = true;
  
  for (; argsleft > 0 and argv_[0][0] == '-'; --argsleft)
  { Char * flgs;
    for (flgs = &argv_[0][1]; *flgs != 0; ++flgs)
      if      (*flgs == '/')
        opts |= M_AOPT;
      else if (*flgs == 'a')
        u_opt = false;
      else if (*flgs == 'd')
        opts |= M_DOPT;
      else if (*flgs == 'x')
        opts |= M_EOPT;
      else
      { explain();
        exit(0);
      }
    ++argv_;
  }

  if (argsleft < 1)
  { explain();
    exit(0);
  }

{ Char * filename = argv_[0];
  Char * path = argsleft > 1 ? argv_[1] : getenv("PATH");
  
  Char * res;


  while ((res = srchpath(filename, path, opts)) != null)
  { printf("%s\n", res);
    if (u_opt)
      break;
  }
}}}
