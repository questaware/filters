#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "build.h"



#define NOOCODES 20

#define MAXLN 200

static char opfname[MAXLN+2];

Char * fromstr = null;
Char * tostr = null;

Char fromtbl[NOOCODES];
Char totbl[NOOCODES];

Vint noocodes = 0;

static void explain()

{ fprintf(stderr, "trans { -s string string } { -c # # } filename*\n");
  exit(0);
}


Bool is_diry(fd)

{	struct stat fstat_;
	int cc = fstat(fd, &fstat_);
	return cc == OK and (fstat_.st_mode & 040000) != 0;
}


Char * trim_str(t)
	Char * t;
{ if (t != null and t[0] == '"')
  { int e = strlen(t)-1;
    if (t[e] == '"')
      t[e] = 0;
    ++t;
  }
  return t;
}

void main(argc, argv)
        Vint    argc;
        Char ** argv;
{ fast Vint argix;
  if (argc <= 1)
    explain();
  
  for (argix = 0; ++argix < argc; )
  { Char * fn = argv[argix];
    if (fn[0] == '-')
    { if      (fn[1] == 's')
      { fromstr = trim_str(argv[++argix]);
	if (fromstr != null)
        { tostr = trim_str(argv[++argix]);
          if (tostr != null)
            continue;
  	}
      }
      else if (fn[1] == 'c')
      { if (noocodes < NOOCODES)
        { fromtbl[noocodes] = atoi(argv[++argix]);
          totbl[noocodes] =   atoi(argv[++argix]);
	  ++noocodes;
	  continue;
  	}
      }
      explain();
    }
    else
    { FILE * chan = fopen(fn, "rb");
      if (chan == null)
        fprintf(stderr, "Cannot open %s\n", fn);
      else if (not is_diry(chan))
      { while (true)
	{ int ch = getc(chan);
	  if (ch == EOF)
	    break;
        { int i;

          for (i = -1; ++i < noocodes; )
            if (fromtbl[i] == ch)
            { ch = totbl[i];
              break;
            }

	  if (ch != 13 && ch >= 0)
	    fputc(ch, stdout);
	}}
        fclose(chan);
      }
    }
  }
}
