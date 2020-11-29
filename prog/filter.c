
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "build.h"


private Int  lineix = 0;
private Int  linect = 0;
private Int  width = 80;
private Int  charlen = 2000000;

private Char * opfn = null;


#define VT 12


#if S_MSDOS == 0
#undef O_BINARY
#define O_BINARY 0
#endif


static void explain()
      { fprintf(stderr, "filter {-w #} {-l #} {-o fn} { file }\n"
                        "     w -- width of max line\n"
                        "     l -- max char len of output\n"
                        " make non printable chars visible\n"
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
	  width = atoi(&flgs[1]);
	else if (argsleft > 0)
	{ --argsleft;
	  if (argsleft <= 0)
	    explain();

	  width = atoi(*++argv_);
	}
      }
      else if (*flgs == 'l')
      { if (in_range(flgs[1], '0', '9'))
	  charlen = atol(&flgs[1]);
	else if (argsleft > 0)
	{ --argsleft;
	  if (argsleft <= 0)
	    explain();
	  charlen = atol(*++argv_);
	}
      }
      else if (*flgs == 'o' && argsleft > 1)
      { --argsleft;
	opfn = *++argv_;
      }
      else
        explain();
    ++argv_;
  }
  lineix = 0;
{ Int charct = 0;
  
  Char * srcfn = argsleft <= 0 ? null : argv_[0];
  struct stat fstat_;
  Cc cc = srcfn == 0 ? -1 : stat(srcfn, &fstat_);
  Int len = cc != OK ? 0 : fstat_.st_size;

  FILE * op = opfn == null ? null : fopen(opfn, "wb");
  int file = srcfn == null ? 0 : open(srcfn, O_RDONLY+O_BINARY);
 
  if (file < 0)
    fprintf(stderr,srcfn == null ? "Cannot open stdin\n"
    				 : "Cannot open file %s\n", srcfn);
  else 
  { if (op == null && opfn != null)
    { fprintf(stderr, "Cannot open output\n");
      exit(0);
    }
    /*if (charlen > len)
        charlen = len; */
  
    while (charct < charlen || 1)
    { 
      Char arr[2];
      Char ch;
      Cc rc = read(file, arr, 1);
      if (rc <= 0)
      { fprintf(stderr, "EOF found\n");
        break;
      }

      ch = arr[0];
      ++lineix;
      charct += 1;
      if ((charct & 0x7fff) == 0)
        fprintf(stderr, ".");
      
      if      (ch == '\n')
      { lineix = 0;
      }
      else if (lineix == width)
      {
        if (op != null)
          putc('\n', op);
        else
          putchar('\n');
        lineix = 0;
      }
      
      ch = ch & 0x7f;
      if (ch <= 'Z'-'@')
      { ch += '@';
        if (op != null)
          putc('^', op);
        else
          putchar('^');
      }

      if (op != null)
        putc(ch, op);
      else
        putchar(ch);
    }
    if (op != null)
      fclose(op);
  }
}}   
