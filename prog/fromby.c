#include        <stdio.h>
#include        <fcntl.h>
#include 	<sys/types.h>
#include 	<sys/stat.h>
#include        <errno.h>
#include        "build.h"


extern Int atol();


void explain()

{  fputs("fromby  { -f{c|l} #} {-b{c|l} #} { file }\n"
	 "   -f{c|l|} #   From character or line #\n"
         "   -b{c|l|} #   By character or line #\n"
         " All one based, default is line\n", stderr
         );
   exit(0);
}


#define SECTSZ 512
#define ATATIME 16


Short trash;


#define MAXLN 200
Char ipline[MAXLN+1];


static  Int  bounds[2] = {0,0x7fffffff};
static  Bool unit_is_line[2] = { false, false };
  


void process_file(Char * fn)

{ Int line = 0;
  FILE * chan = fopen(fn, "r");
      
  if (chan == null)
  { fputs("Cannot open file %s", stderr);
    fputs(fn, stderr);
    fputs("\n", stderr);
    return;
  }

  while (true)
  { Char * ln = &ipline[0];
    Int   ix;
    size_t noch = fread(&ipline[0], 1, MAXLN, chan);
    if (noch == 0 and feof(chan))
      break;

    for (ix = -1; ++ix < noch; )
    { if (bounds[0] <= 0)
      { if (bounds[1] == 0)
	  break;
	putc(ipline[ix], stdout);
	if (not unit_is_line[1] or ipline[ix] == 0x0a)
	  --bounds[1];
      }
      if (not unit_is_line[0] or ipline[ix] == 0x0a)
	--bounds[0];
    }
    if (bounds[1] == 0 or noch != MAXLN and feof(chan))
      break;
  }

  fclose(chan);
}



main(argc, argv)
    int argc;
    char ** argv;
{ if (argc <= 1)
    explain();

{ Vint argsleft;
  Char ** argv_ = &argv[1];

  for (argsleft = argc; --argsleft > 0; )
  { Char * s = &argv_[0][0];

    if (s[0] != '-')
       process_file(s);

    else
     while (*++s != 0)
      if      (*s == 'f' or *s == 'b')
      { Vint wh = *s == 'b';
        switch (*++s)
        { when 'c':  ;
          when 'l':  
          case 0:    unit_is_line[wh] = true;
          otherwise  explain();
        }
        ++argv_;
        --argsleft;
        bounds[wh] = atol(argv_[0]);
        if (wh == 0)
          --bounds[wh];
        break;
      }
      else 
        explain();
    ++argv_;
  }
}}

