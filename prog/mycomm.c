#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "build.h"

#include "../common/h/msfname.h"
#include "../common/h/eprintf.h"


#define MAXLN 200

static char ipline[2][MAXLN+10];

private void explain()

{ fprintf(stderr, "mycomm filename filename\n");
  exit(0);
}

void main(argc, argv)
        Vint    argc;
        Char ** argv;
{ if (argc <= 1)
  { explain();
  }
  
{ Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];

  for (; argsleft > 0; --argsleft)
  { if (argv_[0][0] == '-')
    { explain();
      exit(0);
    }
    else
    { Char * fn1 = argv_[0];
      Char * fn2 = argv_[1];
      if (argsleft <= 1)
        explain();

    { FILE * chan1 = fopen(fn1, "rb");
      FILE * chan2 = fopen(fn2, "rb");

      if      (chan1 == null)
        fprintf(stderr, "Cannot open %s\n", fn1);
      else if (chan2 == null)
        fprintf(stderr, "Cannot open %s\n", fn2);
      else
      { Set ends = 0;
        while (ends != 3)
        { int cc = strcmp(ipline[0], ipline[1]);
          if (cc < 0)
          { puts(ipline[0]);
          }
          if (cc <= 0)
          { if (fgets(ipline[0], MAXLN, chan1) == null)
            { ipline[0][0] = 0x7f;
              ends |= 1;
            }
          }
          if (cc >= 0)
          { if (fgets(ipline[1], MAXLN, chan2) == null)
            { ipline[0][0] = 0;
              ends |= 2;
            }
          }
        }
      }
      break;
    }}
    ++argv_;
  }

}}

