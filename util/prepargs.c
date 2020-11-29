#include "build.h"
#include "../h/prepargs.h"

private Int argc = -1;
private Char ** argv;


#define MININT 0x80000000

public Int args[NARGS];


public Bool moreargs( argc_,argv_, srcfn)
	Short  argc_;
        Char * argv_[];
	Char * *srcfn;
{ Char last_ch = 0;
  Short i,v;

  if (argc == -1)
  { argc = argc_;
    argv = argv_;
  
    for (i = 0;i < NARGS; ++i)
      args[i] = MININT;
  }

  while (--argc > 0)
    if ((++argv)[0][0] != '-')
    { *srcfn = &argv[0][0];
      return true;
    }
    else
    { if (argv[0][1] == 0)
      { *srcfn = "";
        return true;
      }
      i = 1;
      while (true)
      { Char ch = argv[0][i] & 0xdf;
        if (last_ch != 0 and in_range(ch, '0', '9'))
          --i;
        else 
        { if ( not in_range(ch,'A','Z') )
            break;
          last_ch = ch;
        }
        
        v = 0;
        while (true)
        { ch = argv[0][++i];
          if (not in_range(ch, '0', '9'))
            break;
          v = v * 10 + ch - '0';   /* negatives to go in */
        }
        ARGS(last_ch) = v;
      }
    }
  return false;
}  
