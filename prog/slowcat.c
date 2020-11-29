#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "build.h"

extern Int atol();


static void explain()

{ fprintf(stderr, "slowcat [-s cps ] filename*\n"
                  "  -s cps  chars per second\n"
                  "Output characters slowly\n"
         );
  exit(0);
}



int ms_trash;

void millisleep(Int m)

{
#if 1
  Int ct;
  while (--m != 0)
  { for (ct = 150000; --ct != 0; )
    { ms_trash = ct;
    }
  }
#endif
}



main(argc, argv)
    int argc;
    char ** argv;
{ if (argc <= 1)
    explain();
    
{ Char buff[2];
  Int speed = 50;
  Int ct = 0;
  Char ** argv_ = &argv[1];
  int argsleft;

  for (argsleft = argc; --argsleft > 0; )
  { if (argv_[0][0] == '-')
    { Char * s = &argv_[0][0];
      while (*++s != 0)
      { if (*s == 's')
	{ ++argv_;
	  speed = atol(argv_[0]);
	}
	else 
	  explain();
      }
    }
    else
    { char * fn = argv_[0];
      
      int ip  = open(fn, O_RDONLY);
      if (ip < 0)
        fprintf(stderr, "Cannot open %s\n", fn);
      else
	while (1)
	{ int cc = read(ip, &buff[0], 1);
	  if (cc <= 0)
	    break;
	  ++ct;
	  millisleep(1000L/speed);
	  putchar(buff[0]);
	}
    }

    ++argv_;
  }
}} 

