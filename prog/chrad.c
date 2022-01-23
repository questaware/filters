#include   <stdio.h>
#include   <ctype.h>
#include   <fcntl.h>
#include   "build.h"

#include  "../common/h/eprintf.h"


extern Char * strchr();

#define MAX_LN 512

extern Char * realloc();
#undef malloc


static void explain()

{ fputsout(
  "convert val\n"
  "  where val is a number in octal, decimal or hex\n"
          );
  exit(0);
}

void main(argc, argv)
        int     argc;
        Char ** argv;
{ if (argc <= 1)
    explain();
  
{ int ix;
  
  static const Char titles[][14] = {"octal       ", "decimal     ", "hexadecimal",};
  static Char format[] = "%lo";
  
  fputsout("Was V             Octal    Decimal      Hex  Decimal-K  Ascii\n");

  for (ix = -1; ++ix < 3; )
  { Int cval;
    
    sscanf(argv[1], format, &cval);
    
#if S_MSDOS
    eprintf(null,
#else
    printf(
#endif
         "%12s %12lo %10ld  %8lx  %7ld   %c\n", 
                          titles[ix], cval,cval,cval,
			  cval>>10, cval < ' ' ? ' ' : cval);
    format[2] = "dxx"[ix];
  }
}}
