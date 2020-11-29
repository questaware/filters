#include <stdio.h>
#include <ctype.h>
#include "build.h"

#include "../util/h/strutil.h"

extern Char * malloc();

#define SPAREM 50


Char buff[200];
Char lbuff[200];

Int buffeow;
//Int lbuffeow;

Char wordchars[128];
Vint wch_top = 0;

Char nonwordchars[128];
Vint nwch_top = 0;


void explain()

{ fprintf(stderr, "sameword {-#}{-d|{-a|-f|-s|-o|-j}|} {+c ch} }{-c ch} {-nl}\n"
 "  -#      the number of words (DEFAULT 1)\n"
 "  -d      different\n"
 "  -,      All non comma characters are word characters\n"
 "  +c ch   Treat ch as a word character\n"
 "  -c ch   Treat ch as not a word character\n"
 "  -nl     prefix a newline when condition is not met\n" 
 "  -a      Print all lines (DEFAULT)\n"
 "  -f      Print first of same lines\n"
 "  -s      Print subsequent of same lines\n\n" 
 "  -o      omit the first # words when the same, print all lines\n"
 "  -j	    concatenate all but last of same\n"
 " The condition: first # words are the same/(different) to previous line\n"
 " By default A-Za-z0-9_ are word characters\n"
         );
  exit(0);
}



Char * g_last_wd[100];
Vint   g_last_wd_sz[100];


Bool inword(Char ch)

{ Vint ix;
  for (ix = wch_top; --ix >= 0; )
    if (ch == wordchars[ix])
      return true;

  for (ix = nwch_top; --ix >= 0; )
    if (ch == nonwordchars[ix])
      return false;

  return isalpha(ch) or isdigit(ch) or ch == '_';  
}




void main(argc, argv)
    Vint    argc;
    Char ** argv;
{ Vint col;
  Vint sz;
  Bool same_opt = true;
  Bool pfx_opt = false;
  Bool omit_opt = false;
  Bool got_fs_opt = false;
  Bool comma_opt = false;
  int j_opt = 0;
  int fs_opt = 3;
  Int word_ct = 1;

  Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];

  for (; ; --argsleft)
  { if (argsleft > 0 and (argv_[0][0] == '-' or 
                          argv_[0][0] == '+') and argv_[0][1] != 0)
    { if      (argv_[0][1] == 'd')
        same_opt = false;
      else if (argv_[0][1] == 'n' and argv_[0][2] == 'l')
        pfx_opt = true;
      else if (argv_[0][1] == 'j')
      { pfx_opt = true;
        j_opt = 1;
      }
      else if (argv_[0][1] == ',')
        comma_opt = 1;
      else if (argv_[0][1] == 'o')
        omit_opt = 1;
      else if (argv_[0][1] == 'a')
        fs_opt = 3;
      else if (argv_[0][1] == 'f' || argv_[0][1] == 's')
      { if (not got_fs_opt)
        { got_fs_opt = true;
          fs_opt = 0;
        }
        fs_opt |= argv_[0][1] == 'f' ? 1 : 2;
      }
      else if (argv_[0][1] == 'c')
      { Char wh = argv_[0][0];
        --argsleft;
        ++argv_;
        if (argsleft < 0)
          explain();
      { Char ch = argv_[0][0];
        if (ch == '\'')
          ch = argv_[0][1];
        if (wh == '+')
          wordchars[wch_top++] = ch;
        else
          nonwordchars[nwch_top++] = ch;
      }}
      else if (in_range(argv_[0][1], '0', '9'))
      { word_ct = atol(&argv_[0][1]);
        if (word_ct > upper_index(g_last_wd))
        { fprintf(stderr, "-%d is too big", word_ct);
          exit(1);
        }
      }
      else
        explain();
    }
    else
    { Bool lsame = 0;
      Int lct = 0;
      Char * ln;
      Char * fn = argsleft <= 0 ? "-" : argv_[0];
      FILE * chan = fn[0] == '-' and fn[1] == 0 ? stdin : fopen(fn, "r");
      if (chan == null)
      { fprintf(stderr, "Cannot open file %s\n", fn);
        continue;
      }
      
      lbuff[0] = 0;
/*
      if (fs_opt != 0)
        same_opt = true;
*/
      while ((ln = fgets(&buff[0], sizeof(buff), chan)) != null)
      { Char patch;
        Bool same = true;
        /*Vint col*/;
        if (j_opt && ln[0] != 0)
          ln[strlen(ln)-1] = ' ';

	for (col = -1; ++col < word_ct and *(ln = skipspaces(ln)) != 0; )
        { Char * src = ln;
	  if (comma_opt)
            while (*ln != 0 and *ln != ',')
              ++ln;
          else
            while (*ln != 0 and inword(*ln))
              ++ln;

	  buffeow = ln - buff;
	  patch = *ln;
	  *ln = 0;
	  if (g_last_wd[col] == null or strcmp(g_last_wd[col], src) != 0)
            same = false;
	  if (lct == 0 and same_opt)
	    same = false;
	
	{ /* Vint */ sz = ln - src;
	  if (sz == 0)
	  { *ln = patch;
	    break;
	  }
	  if (sz > g_last_wd_sz[col])
	  { if (g_last_wd[col] != null)
	      free(g_last_wd[col]);
	    g_last_wd_sz[col] = sz+SPAREM+1;
	    g_last_wd[col] = malloc(sz+SPAREM+2);
	    if (g_last_wd[col] == null)
	    { fprintf(stderr, "Out of memory\n");
	      exit(1);
	    }
	  }
	  memcpy(&g_last_wd[col][0], src, sz);
	  g_last_wd[col][sz] = 0;
	  *ln = patch;
        }}

	//printf("III %d %s", same, buff);
#if 1        
        if (lbuff[0] != 0)
	{ if (j_opt || pfx_opt && lsame != same_opt)
            printf("\n");

          if (pfx_opt or (fs_opt & 1))
            printf("%s", lbuff);
        }

	lsame = same;
        strcpy(&lbuff[0], buff);
	//lbuffeow = buffeow;

        if (same && (fs_opt & 2))
	{ if (j_opt || pfx_opt && same != same_opt)
            printf("\n");
        { int ix = omit_opt * buffeow;
          printf("%s", &buff[ix]);
          lbuff[0] = 0;
        }}
#endif          
        ++lct;
      } /* line loop */
      if (j_opt)
	printf("\n");

      if (argsleft <= 1)
        break;
    }
    ++argv_;
  }
}
