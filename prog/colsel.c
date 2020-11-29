#include <stdio.h>
#include <string.h>
#include "build.h"


static char nmbuf[50];
static char there[1000];

static int  opt_head = 0;
static char opt_over = 0;
static char opt_sep = ' ';
static int  opt_width = 0;

#define TABSZ 8

#define MAX_COLS 120
#define MAX_PAIRS 120
#define MAX_REC 10000


enum { IN_SPACE, IN_DQ, AFTER_DQ, IN_VAL, };



void putchars(char ch, int n)

{ while (--n >= 0)
    putchar(ch);
}


typedef struct
{ char * pat;
  char * repl;
} Pair;


Pair g_pairstore[MAX_PAIRS];
int pairstore_ct;

int g_cols[MAX_COLS];
Pair * g_pairs[MAX_COLS];

int cols_ct;

char * coltxt[MAX_COLS];

char ipline[MAX_REC+1];




char * repl_pat(char * src, Pair * pats)

{ if (pats != NULL)
  { //printf("Compare %s %s\n", src, pats->pat);
    for (; pats->pat != NULL; ++pats)
    { char * s = src;
      char * t = pats->pat;
      while (*s == *t && *t != 0)
      { ++s; ++t;
      }
      if ((*s == 0 || *s == ',') && *t == 0)
        return pats->repl;
    }
  }
  
  return src;
}




static void explain()
      { fprintf(stderr, "colsel {-#} { # }+ {-f colfile } { file }\n"
                        " -#           number of lines\n"
                        "  #           number of column (1 based)\n"
                        " -f colfile   file giving column # { pat ~>~ repl ~.~ }*\n"
                        "              where pat is a pattern to match, repl its replacement\n"
                        "select columns from csv file\n"
                              
      	       );
        exit(0);
      }


void main (argc,argv)
	Vint argc;
        Char ** argv;
{ Vint argsleft = argc - 1;
  Char ** argv_ = &argv[1];

  int line_clamp = 10000000;
  char * colfile = NULL;

  int max_col_ix = 0;

  for (; argsleft > 0 && (argv_[0][0] == '-' || in_range(argv_[0][0], '0', '9'));
         --argsleft)
  { 
    if      (argv_[0][0] == '-' && argv_[0][1] == 0)
      break;
    else if (argv_[0][0] == '-' && in_range(argv_[0][1], '0', '9'))
      line_clamp = atoi(&argv_[0][1]);
    else if (in_range(argv_[0][0], '0', '9'))
    { int v;
      if (cols_ct >= upper_index(g_cols))
        explain();
      v = atoi(argv_[0]);
      if (v == 0)
        explain();
      g_cols[cols_ct++] = v-1;
      if (v-1 > max_col_ix)
        max_col_ix = v-1;
    }
    else if (argv_[0][1] == 'f' && argsleft > 1)
    { --argsleft;
      ++argv_;
      colfile = argv_[0];
    }
    else 
      explain();
    ++argv_;
  }

  if (colfile != NULL)
  { FILE * cip = fopen(colfile, "r");
    if (cip == NULL)
      fprintf(stderr, "Cannot open colfile %s\n", colfile);
    else
    { while (1)
      { char * ln = fgets(&ipline[0], sizeof(ipline)-1, cip);
        if (ln == null)
          break;
        if (ln[0] == 0 || ln[0] == '\n')
          continue;
      { int opc = pairstore_ct;
        int col = atoi(ln);
        if (cols_ct >= upper_index(g_cols))
	{ fprintf(stderr, "Too many col, circa (%d)\n", MAX_COLS);
	  exit(2);
        }
        if (col == 0)
	{ fprintf(stderr, "Illegal entry %s\n", ln);
	  exit(4);
	}

        g_cols[cols_ct++] = col-1;
        
        for ( ; *ln != 0 && ! isspace(*ln); ++ln)
          ;
	while (1)
        { for ( ; *ln != 0 &&   isspace(*ln); ++ln)
            ;
	  //printf("strstr %s", ln);

	{ char * e = strstr(ln, "~>~");
	  if (e == NULL)
	    break;
	{ char * ee = strstr(e, "~.~");
	  if (ee == NULL)
	    ee = e + strlen(e);

	{ char * pat = malloc(e - ln + 1);
	  char * repl = malloc(ee- e + 1);
	  if (pat == NULL || repl == NULL || pairstore_ct >= MAX_PAIRS - 1)
	  { fprintf(stderr, "Too many pairs, circa (%d)\n", MAX_PAIRS - 1);
	    exit(3);
	  } 
	  g_pairstore[pairstore_ct].pat = pat;
	  g_pairstore[pairstore_ct].repl = repl;
	  ++pairstore_ct;
	  pat[e - ln] = 0;
	  repl[ee - e - 3] = 0;
	  strncpy(pat, ln, e - ln);
	  strncpy(repl, e+3, ee - e - 3);
	  //printf("Pat %s Repl %s\n", pat, repl);
	  
	  if (*ee == 0)
	    break;
	  ln = ee+3;
	}}}}
        if (opc != pairstore_ct)
        { g_pairs[cols_ct-1] = &g_pairstore[opc];
	  ++pairstore_ct;
	}
      }}
      fclose(cip);
    }
  }
  
{ int oflo = 0;
  int max = -1;
  int col = -1;
  Bool prot = false;

  Char * srcfn = argsleft <= 0 || argv_[0][0] == '-' ? null : argv_[0];
  FILE * file = srcfn == null ? stdin : fopen(srcfn, "r");
  FILE * op = null;
 
  if (file == null)
    fprintf(stderr,srcfn == null ? "Cannot open stdin\n"
                                 : "Cannot open file %s\n", srcfn);
  else
  { int line_ct = 1;
    for (; line_ct <= line_clamp ;++line_ct)
    { int ix;
      int colix = 0;
      int sl;
      int state = IN_SPACE;
      char * ln = fgets(&ipline[0], sizeof(ipline)-1, file);
      if (ln == null)
        break;
      if (ln[0] == 0 || ln[0] == '\n')
        continue;
        
      memset(coltxt, 0, sizeof(coltxt));
      
      sl = strlen(ln);
      if (ln[sl-1] == '\n')
        ln[sl-1] = 0;
      for (; *ln != 0; ++ln)
      { if      (state == IN_SPACE)
          if      (isspace(*ln))
            ;
          else
          { if (colix >= upper_index(coltxt))
              oflo = 1;
            else 
              coltxt[colix++] = ln;
            if      (*ln == '"')
              state = IN_DQ;
            else if (*ln != ',')
              state = IN_VAL;
          }
        else if (state == IN_VAL)
        { if (*ln == ',')
            state = IN_SPACE;
        }
        else if (state == IN_DQ)
        { if      (*ln == '\\' && *ln != 0)
            ++ln;
          else if (*ln == '"')
            state = AFTER_DQ;
        }
        else if (state == AFTER_DQ)
        { if (*ln == ',')
            state = IN_SPACE;
        }
      } /* state loop */

      for (ix = 0; ; )
      { int jix = g_cols[ix];
        if (jix < colix)
        { char * s = coltxt[jix];
          char * stt = s;
          char sch;
          if (*s == '"')
          { char * es;
            for ( es = s; *++es != 0 && (*es != '"' || es[-1] == '\\'); )
              ;
            sch = *es;
            *es = 0;
            
            fputc(*stt, stdout);
            
          { char * ss = repl_pat(s+1, g_pairs[ix]);
            for ( ; *ss != 0; ++ss)
            { fputc(*ss, stdout);
              if      (*ss == '\\')
              { if (*++ss == 0)
                  break;
                fputc(*ss, stdout);
              }
            }
            if (*ss == '"')
              fputc(*ss, stdout);
            *es= sch;
          }}
          else
          { char * ss = repl_pat(s, g_pairs[ix]);
            for (--ss; *++ss != 0 && *ss != ','; )
              fputc(*ss, stdout);
          }
        }
        if (++ix >= cols_ct)
          break;
        fputc(',', stdout);
      } /* column loop */
      fputc('\n', stdout);
    } /* line loop */
  }
  if (oflo)
    printf("OVERFLOWED\n");
  return 0;
}}
