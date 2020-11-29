#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "build.h"





private void explain()

{ fprintf(stderr, "isprime #\n");
  exit(0);
}


void main(argc, argv)
        Vint    argc;
        Char ** argv;
{ if (argc != 2)
  { explain();
  }

{ Int val = atol(argv[1]);
  Int sq = 1;
  Vint log = 1;
  while (sq < val)
  { sq *= 2;
    log += 1;
  }

{ Int root = 1 << (log / 2 + 1);
  Int i;

  for (i = 1; (i+=2) <= root; )
    if ((val % i) == 0)
    { printf("LCD %ld is %ld\n", val, i);
      exit(1);
    }

  printf("%ld is PRIME\n", val);
}}}

