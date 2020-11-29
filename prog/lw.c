#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "version.h"



#define BUFSZ 1024

static char buf[BUFSZ+2];

#define DIRSEPCHR '/'

#define DFLT_LINES 18

private void explain()

{ fprintf(stderr, "lw [ -# ] [ -i str ] [ -w str ] filename*\n");
  fprintf(stderr, "   -#     -- nth last line\n");
  fprintf(stderr, "   -i str -- string to ignore\n");
  fprintf(stderr, "   -w str -- characters within words\n");
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
  
  Char * ignstr = null;
  Char * wstr = "";
  Char * cmdnm_ = argv[0];
  Char * cmdnm;
  for (cmdnm = &cmdnm_[strlen(cmdnm_)]; --cmdnm >= cmdnm_; )
    if (*cmdnm == DIRSEPCHR)
	break;
{ Bool tail_opt = strcmp(&cmdnm[1], "tail") == 0 or 
		  strcmp(&cmdnm[1], "tl")   == 0;     /* last n lines wanted */
  Int lastn = tail_opt ? DFLT_LINES : 1;

  for (; argsleft > 0; --argsleft)
  { if (argv_[0][0] == '-')
    { if      (in_range(argv_[0][1], '0', '9'))
        lastn = atol(&argv_[0][1]);
      else if (argv_[0][1] == 'i')
      { ignstr = *++argv_;
        if (ignstr[0] == 0)
          ignstr = "`z`";
        --argsleft;
      }
      else if (argv_[0][1] == 'w')
      { wstr = *++argv_;
        --argsleft;
      }
      else
      { explain();
        exit(0);
      }
    }
    else
    { Char * fn = argv_[0];
      Id ip = open(fn , O_RDONLY);
      if (ip < 0)
        fprintf(stderr, "Cannot open %s\n", fn);
      else
      { Int resix = BUFSZ;
        Int line_ct = lastn;
        Int ign_ix = -1;
        Int offset = lseek(ip, -1, 2);
      
        for (; offset >= 0; offset = lseek(ip, -2, 1))
	{ Char ch;
	  Int ct = read(ip, &ch, 1);
	  if (ct <= 0)
	    break;
	  if (line_ct <= 0)
	  { Char * w = "X";
	    if (not isdigit(ch) and not isalpha(ch) and ch != '_')
	    { for (w = &wstr[-1]; *++w != 0 and *w != ch; )
	        ;
	    }
	    
	    if (*w != 0)			/* within a word */
	    { if (--resix >= 0)
	        buf[resix] = ch;
	    }
	    
	    --ign_ix;
	    if (ign_ix >= 0)
	    { if      (ignstr[ign_ix] != ch)
	        ign_ix = -1;
	      else if (ign_ix == 0)
	      { line_ct = 1;			/* one more line */
	        ign_ix = -1;
	      }
	    }
	    if (line_ct <= 0 and *w == 0)
	      break;
	  }
	  if (ch == '\n')
	  { --line_ct;
	    if (tail_opt and line_ct < 0)
	      break;
	    resix = BUFSZ;
	    if (ignstr != null)
	      ign_ix = strlen(ignstr);	    
	  }
	}
	if (resix < 0)
	  resix = 0;
	if (not tail_opt)
	  printf("%s", &buf[resix]);
	else
	{ Char ch;
	  if (offset < 0)
	    lseek(ip, 0, 0);
	  while (read(ip, &ch, 1) == 1)
	    putchar(ch);
	}
        close(ip);
      }
    }

    ++argv_;
  }

}}}
