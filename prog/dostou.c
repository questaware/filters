#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "build.h"

#include "../common/h/msfname.h"


#define MAXLN 200

static char ipline[2 * MAXLN+10];
static char opfname[MAXLN+2];

private void explain()

{ fprintf(stderr, "dostou filename*\n");
  exit(0);
}


Bool isdir_y(fd)
	int fd;
{	struct stat fstat_[3];
	int cc = fstat(fd, &fstat_[1]);

	return cc == OK and (fstat_[1].st_mode & 040000) != 0;
}

void main(argc, argv)
	Vint	argc;
	Char ** argv;
{ if (argc <= 1)
    explain();
  
{ Int argsleft = argc;
  Char ** argv_ = &argv[1];

  for (; --argsleft > 0; ++argv_)
  { if (argv_[0][0] == '-')
      explain();
  { int ch;
    Char * fn = argv_[0];
    FILE * chan = fopen(fn, "rb");
    Char * fn_ = &fn[strlen(fn)];
    while (--fn_ > fn and *fn_ != '/')
      ;
    strcpy(&opfname[0], fn);
    strcpy(&opfname[fn_ - fn], "pjsdtu");
  { int doit = false;
    FILE * op = fopen(opfname, "wb");
    if	    (op == null)
      fprintf(stderr, "Cannot open temp file");
    else if (chan == null)
      fprintf(stderr, "Cannot open %s\n", fn);
    else
    { if (not isdir_y(fileno(chan)))
	while ((ch = getc(chan)) != EOF)
	  if      (ch == 'M'-'@')
	    doit = true;
	  else if (ch != 'Z'-'@')
	    fputc(ch, op); 

      fclose(op);
      fclose(chan);
      
      if (!doit)
        unlink(opfname);
      else
      {
#if S_MSDOS
        Cc cc = replace_file(fn, opfname);
        if (cc != OK)
	  fprintf(stderr, "Failed to save to %s\n", fn);
#else
        sprintf(&ipline[0], "mv %s %s", opfname, fn);
        system(ipline);
#endif
        printf("Replaced %s\n", fn);
      }
    }
  }}}

}}
