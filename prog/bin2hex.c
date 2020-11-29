
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "build.h"

extern Int atol();


static void explain()

{ puts("bin2hex {-c} {-2} {-w} {-f offs} {-p pattern } filename\n"
                  "  -c      Output in ascii\n"
                  "  -f offs From offset\n"
                  "  -h      output offsets\n"
                  "  -p pat  Not implemented\n"
                  "  -2      Swap bytes\n"
                  "  -w      Wide display\n"
                  "  -C      Exclude hex display, include ascii\n"
      );  
  exit(0);
}


main(argc, argv)
    int argc;
    char ** argv;
{ if (argc <= 1)
    explain();
    
{ Int foffs = 0;
  char swap = 0;
  short bytewidth = 15;
  Char * pat = null;
  Bool asc_opt = false;
  Bool hdr_opt = false;
  Bool terse = false;
  Vint argsleft;
  Char ** argv_ = &argv[1];
  for (argsleft = argc; --argsleft > 0; )
  { if (argv_[0][0] != '-')
      break;

  { Char * s = &argv_[0][0];
    while (*++s != 0)
    { switch (*s)
      { case '2':
          swap = 2;
        when 'w':
          bytewidth = 31;
        when 'C':
          terse = true;
        case 'c':
          asc_opt = true;
        when 'f':
          ++argv_;
          foffs = atol(argv_[0]);
        
        when 'h':
          hdr_opt = true;
      
        when 'p':
          ++argv_;
          pat = argv_[0];
        otherwise
          explain();
      }
    }
    ++argv_;
  }}

{ char * fn = argv_[0];
  int ip = argsleft == 0 ? 0 : open(fn,O_RDONLY+O_BINARY);
  if (ip < 0)
  { puts("cannot open i/p");
    puts(fn);
    puts("\n");
    exit(-1);
  }

{ Int offs = -1;
  Bool hdr = true;
  short state = 0;
  register short val = 0;  
  short ct = 0;
  unsigned char buff[32];
  static char hexchars [] = "0123456789abcdef";
  
  while (1)
  { int ix;
    char ch;
    if (read(ip, &buff[ct & bytewidth], 1) <= 0)
      break;

    ++offs;
    if (offs < foffs)
      continue;

    ch = buff[ct & bytewidth];
    ++ct;
    if (state == 0)
      val += ch & 0xff;
    else
      val += (short)ch << 8;
        
    state += 1;

    if (hdr and hdr_opt)
    { hdr = false;
    /*printf("%x: ", offs);*/
    
      for (ix = 24; (ix -= 4) >= 0; )
        putchar(hexchars[(offs >> ix) & 15]);
      puts(": ");
    }

    if (state >= 2)
    { 
      if (not terse)
	if	(swap == 2)
	{ putchar(hexchars[(val >> 12) & 0xf]);
	  putchar(hexchars[(val >>  8) & 0xf]);
	  putchar(hexchars[(val >>  4) & 0xf]);
	  putchar(hexchars[(val >>  0) & 0xf]);
	}
	else if (swap == 0)
	{ putchar(hexchars[(val >>  4) & 0xf]);
	  putchar(hexchars[(val >>  0) & 0xf]);
	  putchar(hexchars[(val >> 12) & 0xf]);
	  putchar(hexchars[(val >>  8) & 0xf]);
	}
      if      (bytewidth == 31 and (ct & 2))
        ;
      else if (ct & bytewidth)
      { if (not terse)
          putchar(' ');
      }
      else
      { if (asc_opt)
        {
          if (not terse)
            putchar(' ');
          for (val = -1; ++val <= bytewidth; )
          { if (buff[val] < ' ')
              buff[val] = '.';
            putchar(buff[val]);
          }
        }
        putchar('\n');
        hdr = true;
      }
      state = 0;
      val = 0;
    }
  }
}}}}
