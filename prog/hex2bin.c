#include <stdio.h>
#include <ctype.h>


main(argc, argv)
    int argc;
    char ** argv;
{ if (argc <= 1)
  {  fprintf(stderr, "hex2bin {-2} filename\n");  
     exit(0);
  }

{ char swap = 0;
  if (argv[1][0] == '-' && argv[1][1] == '2')
  { swap = 1;
     ++argv;
  } 
  
{ char * fn = argv[1];
  FILE * ip = fopen(fn, "r");
  if (ip == NULL)
  { fprintf(stderr, "cannot open i/p %s", fn);
    exit(-1);
  }

{ short state = 0;
  short val = 0;  
  
  while (1)
  { int ch = fgetc(ip);
    if (ch == EOF)
      break;

    if (! isspace(ch))
    { static char shft [] = {4,0,12,8};
      static char shft_[] = {12,8,4,0};

      if      ((unsigned)(ch-'0')<=9)
	ch -= '0';
      else if (ch >= 'A' && ch <= 'F')
	ch -= 'A' - 10;
      else if (ch >= 'a' && ch <= 'f')
	ch -= 'a' - 10;

      val += (short)ch << (swap ? shft [state]
	                        : shft_[state]);
      state += 1;
    }

    if (state >= 4)
    { putchar(val >> 8);
      putchar(val);
      state = 0;
      val = 0;
    }
  }
}}}} 
