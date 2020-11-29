#include <stdio.h>
#include "build.h"


static char nmbuf[50];
static char there[10000];

static int  opt_head = 0;
static char opt_over = 0;
static char opt_sep = ' ';
static int  opt_width = 0;

#define TABSZ 8


void putchars(char ch, int n)

{ while (--n >= 0)
    putchar(ch);
}



static void explain()
      { fprintf(stderr, "crumple {-h #} {-c char} { file }\n"
                        "  -h #    -- Dont protect # head lines\n"
                        "  -o char -- Dont protect lines starting char\n"
                        "  -c char -- Separate fields using char\n"
                        "  -w #    -- Separate fields with # chars\n"
                        "minimise whitespace columns\n"
                              
      	       );
        exit(0);
      }

void main (argc,argv)
	Vint argc;
        Char ** argv;
{ Vint argsleft = argc - 1;
  Char ** argv_ = &argv[1];

  for (; argsleft > 0 and argv_[0][0] == '-'; --argsleft)
  { 
    if      (argv_[0][1] == 0)
      break;
    else if (argv_[0][1] == 'h')
    { --argsleft;
      if (argsleft <= 0)
        explain();

      opt_head = atoi(*++argv_);
      if (opt_head <= 0)
        explain();
    }
    else if (argv_[0][1] == 'w')
    { --argsleft;
      if (argsleft <= 0)
        explain();

      opt_width = atoi(*++argv_) - 1;
      if (opt_width < 0)
        explain();
    }
    else if (argv_[0][1] == 'o')
    { --argsleft;
      if (argsleft <= 0)
        explain();

      ++argv_;
      opt_over = argv_[0][0];
    }
    else if (argv_[0][1] == 'c')
    { --argsleft;
      if (argsleft <= 0)
        explain();

      ++argv_;
      opt_sep = argv_[0][0];
    }
    else
      explain();
    ++argv_;
  }
  
{ int max = -1;
  int col = -1;
  Bool prot = false;

  Char * srcfn = argsleft <= 0 || argv_[0][0] == '-' ? null : argv_[0];
  FILE * file = srcfn == null ? stdin : fopen(srcfn, "r");
  FILE * op = null;
 
  if (file == null)
    fprintf(stderr,srcfn == null ? "Cannot open stdin\n"
                                 : "Cannot open file %s\n", srcfn);
  else
  { if (srcfn == null)
    { sprintf(&nmbuf[0], "/tmp/crump%d", getpid());
      op = fopen(nmbuf, "w");
      if (op == null)
      { fprintf(stderr, "Cannot open temp file (fatal\n");
        exit(1);
      }
    }

  { Int hd = opt_head;
  
    while (true)
    { ++col;
    { Short ch = getc(file);
      if (ch < 0)
        break;
      if (op != null)
        fputc(ch, op);
      if (ch == '\n')
      { col = -1;
        --hd;
        prot = false;
      }
      else if (ch == '\r')
        col = -1;
      else if (col == 0 and ch == opt_over)
        prot = true;
      else if (prot == 0 and hd <= 0)
      { if (col > max)
          max = col;
        if      (ch == ' ')
          ;
        else if (ch == '\t')
          col = ((col + 8) & ~(TABSZ-1)) - 1;
        else
          if (col < sizeof(there))
            there[col] = 1;
      }
    }}
    
#if 0
    for (col = -1; ++col <= max; )
    { char c = there[col] ? 'Y' : ' ';
      putchar(c);
    }
    putchar('\n');
    putchar('\r');
#endif
    
    if (srcfn != null)
      fseek(file, 0, SEEK_SET);
    else
    { fclose(op);
      fclose(file);
      file = fopen(nmbuf, "r");
      if (file == null)
      { fprintf(stderr, "Cannot reopen temp file!\n");
        exit(1);
      }
    }

    col = -1;
    
    while (true)
    { ++col;
    { Short ch = getc(file);
      if (ch < 0)
        break;
      if      (ch == '\n')
      { col = -1;
        putchar(ch);
      }
      else if (ch == '\r')
      { col = -1;
        putchar(ch);
      }
      else if (ch == '\t')
      { int lim = ((col + 8) & ~(TABSZ-1)) - 1;
        int col_ = col;
        int ct = 0;
        for ( ; col <= lim; ++col)
          if (col >= sizeof(there)-1 or there[col] or there[col+1])
            ++ct;
        if      (ct = 0)
          ;
        else if (opt_sep == 0 and ct == lim + 1 - col_)
          putchar('\t');
        else
          for ( col = col_; col <= lim; ++col)
          { if (col >= sizeof(there)-1 or there[col] or there[col+1])
              putchar(opt_sep);
            if (opt_width != 0)
              if (col < sizeof(there)-1 and not there[col] and there[col+1])
	        putchars(opt_sep, opt_width);
          }
        col = lim;
      }
      else
      { if (ch == ' ')
          ch = opt_sep;
        if (col >= sizeof(there)-1 or there[col] or there[col+1])
          putchar(ch);
        if (opt_width != 0)
          if (col < sizeof(there)-1 and not there[col] and there[col+1])
	    putchars(opt_sep, opt_width);
      }
    }}
  }}
}}

