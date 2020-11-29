#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "build.h"


extern Int atol();

#define MAXLN 1024

static char buf[MAXLN+2];

private void explain()

{ fprintf(stderr, "tail {+#} filename*\n   +#   pass lines after line #\n");
  exit(0);
}


Bool is_diry(fd)

{	struct stat fs;
	int cc = fstat(fd, &fs);
	return cc == OK and (fs.st_mode & 040000) != 0;
}

void main(argc, argv)
	int	argc;
	Char ** argv;
{ if (argc <= 1)
    explain();

{ Int headct = 0;

  Int argsleft = argc;
  Char ** argv_ = &argv[1];

  for (; --argsleft > 0; ++argv_)
  { if (argv_[0][0] == '+')
    { if (not in_range(argv_[0][1], '0', '9'))
        explain();
      headct = atol(&argv_[0][1]);
    }
    else
    { Char *ln;
      Int ct = 0;
      FILE * chan = fopen(argv_[0], "r");
      if (chan == null)
	fprintf(stderr, "Cannot open file %s\n", argv_[0]);
      else
      { while ((ln = fgets(&buf[0], MAXLN, chan)) != null)
	  if (++ct >= headct)
	    fputs(ln, stdout);
	fclose(chan);
      }
    }
  }
}}

