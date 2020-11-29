#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "build.h"

#include "../common/h/eprintf.h"


#define MAXLN 200

static char ipline[2 * MAXLN+10];
static char opfname[MAXLN+2];

private void explain()

{ fprintf(stderr, 
"sieve {-zZ ch } filename*\n");
"   -z  L  -- letter L  terminates\n"
"   -z ^L  -- letter ^L terminates\n"
"   -Z  L  -- etc, output terminator\n"
"   Remove chars ^@ to ^K, ^M to ^Z"
	 );
  exit(0);
}


Bool is_diry(fd)

{	struct stat fstat_;
	int cc = fstat(fd, &fstat_);
	return cc == OK and (fstat_.st_mode & 040000) != 0;
}

Char * stdinarg[] = { "" };

int termch = -1; 

int output_term = false;

void main(argc, argv)
	Vint	argc;
	Char ** argv;
{ Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];

  if (argsleft == 0)
  { argsleft = 1;
    argv_ = &stdinarg;
  }
  
  for (; argsleft > 0; --argsleft)
  { if (argv_[0][0] == '-')
    { if (argv_[0][1] == 'z' or 
	  argv_[0][1] == 'Z')
      { if (argv_[0][1] == 'Z')
	  output_term = true;
	--argsleft;
	++argv_;
	termch = argv_[0][0];
	if (termch == '^')
	  termch -= '@';
      }
      else
	explain();
    }
    else
    { Char * fn = argv_[0];
      FILE * chan = fn[0] == 0 ? stdin : fopen(fn, "rb");
      Char * fn_ = &fn[strlen(fn)];
      while (--fn_ > fn and *fn_ != '/')
	--fn_;
      strcpy(&opfname[0], fn);
      strcpy(&opfname[fn_ - fn], "pjsdtu");
    { FILE * op = fn[0] == 0 ? stdout : fopen(opfname, "wb");
      if      (op == null)
	fprintf(stderr, "Cannot open temp file");
      else if (chan == null)
	fprintf(stderr, "Cannot open %s\n", fn);
      else if (not is_diry(chan))
      { 
	while (true)
	{ int ch = getc(chan);
	  if (ch == EOF)
	    break;
	  if (ch == termch)
	  { if (output_term)
	      fputc(ch, op);
	    break;
	  }
	  if (ch >  'Z' - '@' or 
	      ch == 'L' - '@')
	    fputc(ch, op);
	}
	fclose(op);
	fclose(chan);
	if (fn[0] != 0)
	{ 
#if S_MSDOS
	  eprintf(&ipline[0], "move %s %s", opfname, fn);
#else
	  eprintf(&ipline[0], "mv %s %s", opfname, fn);
#endif
	  system(ipline);
	}
      }
    }}

    ++argv_;
  }

}
