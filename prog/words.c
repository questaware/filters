#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "build.h"


#define SHO_STATE 0



#define MAXLN 1024
#define MAXINDENT 80

static char ipline[MAXLN+2+MAXINDENT];

void explain()
  
{ fputs( 
"words {-s <seps>} {-r} {-k} {-f <source>} {-t <target>} {-d} {-q} filename*\n"
"  -s <seps>   The separator characters\n"
"  -r          Remove separators\n"
"  -k          Keep excess spaces\n"
"  -f <source> The characters to be replaced\n"
"  -t <target> The characters which replace\n"
"  -d          leave \"abc def\" unchanged\n"
"  -q          leave 'abc def' unchanged\n"
" if -f then replace chars in source by that in target at same offset\n"
" if -s then output words on separate lines white space is always a separator.\n"
" of <seps>, <source>, <target> :\n"
" . can be surrounded by optional quotes \" or \'\n"
" . \\\\, \\ddd, \\n, \\t, \\r are recognised\n",
         stderr);
  exit(0);
}


Vint line_width = 80;

int d_opt = -1;
int q_opt = -1;
Bool k_opt;
Bool r_opt;

Char * seps = "";
Char * source = "";
Char * target;
int target_len;
int seps_len;
int source_len;



Bool strip_esc_len;		/* aux result */

char * strip_esc(char * src)

{ char qch = 0;
  int slen = 0;
  int esc_ct = 0;
  char * res;
  char * t;
  register char * s;
  for (s = src-1; *++s != 0; ++slen)
    if (*s == '\\')
      esc_ct += 1;
  
  strip_esc_len = 0;

  if (src[0] == '\'')
  { qch = '\'';
    ++src;
  }
  else
    if (esc_ct == 0)
      return src;
    
  res = (char*)malloc(slen+1);
  t = res-1;
  
  for (s = src-1; *++s != 0 && *s != qch;)
  { *++t = *s;
    if (*s == '\\')
    { if (s[1] == 0)
        break;
      if (in_range(s[1], '0', '7') and 
          in_range(s[2], '0', '7') and 
          in_range(s[3], '0', '7')
         )
      { *t = (((s[1]-'0') * 8) + (s[2]-'0')) * 8 + s[3] - '0';
        s += 3;
      }
      else if (s[1] == 'n')
      { *t = '\n';
        s += 1;
      }
      else if (s[1] == 'r')
      { *t = '\r';
        s += 1;
      }
      else if (s[1] == 't')
      { *t = '\t';
        s += 1;
      }
      else if (s[1] == '\\')
      { *t = '\\';
        s += 1;
      }
    }
  }
  
  *++t = 0;
  strip_esc_len = t - res;
  return res;
}

int ix_in_str(const char * src, int len, char ch)

{ int ix;
  for (ix = -1; ++ix < len; )
    if (ch == src[ix])
      return ix;
  return -1;
}

int main(argc, argv)
  short   argc;
  Char ** argv;
{ Bool in_str = false;
  Bool in_char = false;
  register int ch;
  
  Int argsleft = argc - 1;
  Char ** argv_ = &argv[0];
  
  for ( ; ; --argsleft)
  { ++argv_;
    
    if (argsleft > 0 && argv_[0][0] == '-' && argv_[0][1] != 0)
    { ch = argv_[0][1];
      if      (ch == 's' and argsleft >= 1)
      { seps = strip_esc(argv_[1]);
        seps_len = strip_esc_len;

      /*printf("SEPS >%s<\n", seps);*/
        ++argv_;
	--argsleft;
	continue;
      } 
      else if (ch == 'f' and argsleft >= 1)
      { source = strip_esc(argv_[1]);
        source_len = strip_esc_len;
      /*printf("AFROM %d >%s<\n", source_len, argv_[1]);*/
	++argv_;
	--argsleft;
	continue;
      } 
      else if (ch == 't' and argsleft >= 1)
      { target = strip_esc(argv_[1]);
	target_len = strip_esc_len;
      /*printf("ATO %d >%s<\n", target_len, argv_[1]);*/
	
        ++argv_;
	--argsleft;
	continue;
      } 
      else if (ch == 'd')
      {	d_opt = '"';
	continue;
      }
      else if (ch == 'q')
      {	q_opt = '\'';
	continue;
      }
      else if (ch == 'k')
      {	k_opt = true;
	continue;
      }
      else if (ch == 'r')
      {	r_opt = true;
	continue;
      }
      else if (ch == 0)
	;
      else
      { explain();
      }
    }
    else
    { Char * fn = argv_[0];
      if (argsleft <= 0)
        fn = "-";
    
    { int  spct = 0;
      Char pch;
      FILE * chan = strcmp(fn, "-") == 0 ? stdin : fopen(fn, "rb");
    /*printf("OPen %s\n", fn);*/
      if (chan == null)
      { fputs("Cannot open file ", stderr);
        fputs(fn, stderr);
        fputs("\n", stderr);
        continue;
      }
      
      for (; true; pch = ch)
      { 
        ch = fgetc(chan);
        if (ch < 0)
          break;
        
      /*printf("X %c X\n", ch);*/
  
      { int ix = ix_in_str(source, source_len, ch);
        if (ch == d_opt && pch != '\\' and q_opt == 0)
          in_str = ! in_str;
        if (ch == q_opt && pch != '\\' and d_opt == 0)
          in_char = ! in_char;
          
        if (ix >= 0)
        { 
  	if (ix >= target_len)
  	   continue;
  	ch = target[ix];
        }
  
        if (seps[0] == 0 or in_char or in_str)
        { 
          putchar(ch);
  	continue;
        }
  
        if (isspace(ch) or ix_in_str(seps, seps_len, ch) >= 0)
        { spct += 1;
  
  	if (k_opt or !r_opt and !isspace(ch))
  	{ putchar (ch);
  	  spct = 1;
  	}
        }
        else
        { if (spct > 0)
  	  putchar('\n');
          spct = 0;
  	putchar(ch);
        }
      }}
      if (spct > 0)
        putchar('\n');
      
      fclose(chan);
      if (argsleft <= 0)
        break;
    }}
  }

  return 0;
}
