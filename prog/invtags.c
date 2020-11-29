#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "build.h"
#include "../common/h/msdir.h"
#include "../util/h/strutil.h"


extern char * skipspaces();
extern char * strmatch();


#define MAXLN 1024

static char fnline[FILENAMESZ+2];
static char ipline[MAXLN+2];
static char labelline[MAXLN+2];
static char label[MAXLN];


#define M_INCHAR 1
#define M_INSTR  2
#define M_INCMT  4
#define M_INDRV  8
#define M_INESC 16


  Char * termstr = "(";

  Bool at_opt = false;
  Bool d_opt = false;
  Bool e_opt = false;
  Bool p_opt = false;
  Char * pname = "+";
  Bool q_opt = false;
  Bool r_opt = false;
  Bool s_opt = false;
  Bool u_opt = false;
  Bool v_opt = false;
  Bool x_opt = false;
  int  nest_lvl = 0;

private void explain()

{ fprintf(stderr, 
			"invtags filename*\n"
			"   -a -- find name[\n"
			"   -d -- find declarations i.e pn(){}, #define pn()\n"
			"   -e -- show all defines\n"
			"   -r -- list references name()\n"
			"-p origfn -- extract from cpp orginfn\n"
			"   -q -- quiet\n"
			"   -s -- Dont show filename\n"
			"   -u -- output labels in upper case\n"
			"   -v -- verbose\n"
			"   -x -- include externs\n"
			"   -@ -- use list of files\n"
                        "   -# -- find them nested in # braces (DEFAULT 0)\n"
			"No a-z is similar to -r\n");
}

private void do_it(eny, chan)
	Char * eny;
	FILE * chan;
{ Char * format = s_opt ? "%s\n" 	  :
		  v_opt ? "%s\t%s\t?%s" : "%s\t%s\n";
  Bool isextern = false;
  Bool ignoring = false;
  Bool contn = false;
  Char wantch = 0;
  Short brkt_lvl = 0;
  Short sq_lvl = 0;
  Short p_lvl = 0;
  Shortset mode = 0;

  while (true)
  { Char * ln = fgets(&ipline[0], MAXLN, chan);
    if (ln == null)
      break;
    ln = skipspaces(ln);
  { Short slen = strlen(ln);
    if (slen <= 1)
      continue;
  { Char * lne = &ln[slen-2];
    Bool hcont = *lne == '\\';
    if (contn)
    { contn = hcont;
      continue;
    }
    contn = hcont;

    if (*ln == '#')
    { for (++ln; isspace(*ln); ++ln)
        ;
      if      (p_opt and atoi(ln) != 0)
      { while (*ln != 0 and *ln != '"')
         ++ln;
      { Char * lnn;
        for (lnn = ln; *++lnn != 0 and *lnn != '"'; )
          ;
        *lnn = 0;
        if (ln[0] == '"')
          ignoring = strcmp(&ln[1], pname) != 0;
      }}
      else if (*strmatch("define", ln) == 0)
      { Char * wordstt = skipspaces(&ln[6]);
        
        for (ln = wordstt; isalpha(*ln) or *ln == '_'; ++ln)
	  ;
	  
        if (not e_opt)
	  if (*ln != '(')
	    continue;
      { Char sch = *ln;
	*ln = 0;
	strcpy(&label[0], wordstt);
	*ln = sch;
	printf(format, label, eny, ipline);
	label[0] = 0;
      }}
      continue;
    }

    if (not ignoring)
    for (;*ln != 0; ++ln)
    { fast Char lch = *ln;

      if      (mode & M_INESC)
	mode &= ~M_INESC;
      else if (lch == '\\')
	mode |= M_INESC;
      else if (lch == '\'' and not (mode & (M_INSTR+M_INCMT)))
	mode ^= M_INCHAR;
      else if (lch == '"' and not (mode & (M_INCHAR+M_INCMT)))
	mode ^= M_INSTR;
      else if (lch == '/' and ln[1] == '*' and 
		not (mode & (M_INCHAR+M_INSTR+M_INCMT)))
      { ++ln;
        mode |= M_INCMT;
      }
      else if (lch == '*' and ln[1] == '/' and (mode & M_INCMT))
      { ++ln;
	mode &= ~M_INCMT;
      }
      else if (lch == '\n')
	mode &= ~(M_INSTR+M_INCHAR);
      
      else if ((mode & (M_INSTR+M_INCHAR+M_INCMT)) == 0)
      { if      (lch == '{')
	  brkt_lvl += 1;
        else if (lch == '}')
	  brkt_lvl -= 1;
        else if (lch == '[')
	  sq_lvl += 1;
        else if (lch == ']')
	  sq_lvl -= 1;
        else if (lch == '(')
	  p_lvl += 1;
        else if (lch == ')')
	  p_lvl -= 1;
	else if (lch == ';')
	  isextern = false;
	if (lch == wantch)
	{ if (wantch == '{' and label[0] != 0)
          { if (u_opt)
            { int ix;
              for (ix = -1; label[++ix] != 0; )
                label[ix] = toupper(label[ix]);
            }
	    printf(format, label, eny, labelline);
	    label[0] = 0;
	  }
	  wantch = 0;
	}
         	
        if (r_opt)
        { if (ln[0] == '(' and ipline[0] != '#')
          { Char * b;
            Char * eow;
            Char patch;
            for (b = ln; --b > ipline and isspace(*b); )
                ;
            eow = b+1;
            for (++b; --b >= ipline and (isalpha(*b) or isdigit(*b) or *b =='_'); )
              ;
            ++b;
            patch = *eow;
	    *eow = 0;
	    if (*b == 0 or
	    	isextern and not x_opt or
	        strcmp("if",b) == 0 or
	        strcmp("for",b) == 0 or
	        strcmp("else",b) == 0 or  /* else (x)->fld = 0; */
	        strcmp("while",b) == 0 or
	        strcmp("void",b) == 0 or
	        strcmp("atol",b) == 0 or
	        strcmp("atoi",b) == 0 or
	        strcmp("printf",b) == 0 or
	        strcmp("fprintf",b) == 0 or
	        strcmp("sprintf",b) == 0 or
	        strcmp("fclose",b) == 0 or
	        strcmp("close",b) == 0 or
	        strcmp("fopen",b) == 0 or
	        strcmp("open",b) == 0 or
	        strcmp("strcmp",b) == 0 or
	        strcmp("strcpy",b) == 0 or
	        strcmp("memcpy",b) == 0 or
	        strcmp("memcmp",b) == 0 or
	        strcmp("fgets",b) == 0 or
	        strcmp("void",b) == 0 or
	        strcmp("switch",b) == 0 or
	        strcmp("return",b) == 0)
	        ;
	    else
	      printf("%s\n", b);
            *eow = patch;
          }
        }
        else if (not isalpha(lch) and lch != '_')
	{ if (wantch != 0)
          { 
            if (lch == '*' or lch == ',' or lch == ')' or 
                lch == '[' or lch == ']' or sq_lvl > 0 or
                isspace(lch))
	      ;
            else if (lch == ';' && ln[1] != ';')
              ;
	    else
	    { if (not q_opt)
	       fprintf(stderr, "Suppressed %s in %s by %c\n", label, eny, lch);
	      wantch = 0;
	    }
          }
        }
        else
        { Char * ln_;
          Char * ln__;
          for (ln_= ln; isalnum(*ln_) or *ln_=='_'; ++ln_)
            ;
	  
	  if (strmatch(ln, "extern") == ln_)
	    isextern = true;
          
          for (ln__ = ln_; isspace(*ln__); ++ln__)
            ;

          if      (ln__[0] != *termstr || brkt_lvl > nest_lvl)
            --ln__;
          else 
          { if (ln__[0] == ')') 
               p_lvl += 1; 
            else
               sq_lvl += 1;
            if (not e_opt and (brkt_lvl == 0 or not d_opt))
	    { Char sch = *ln_;
      
              *ln_ = 0;
	      if (x_opt >= isextern and
	          strcmp(ln, "if") != 0 and
	          strcmp(ln, "for") != 0 and
	          strcmp(ln, "else") != 0 and
	          strcmp(ln, "while") != 0 and
	          strcmp(ln, "void") != 0 and
	          strcmp(ln, "switch") != 0 and
	          strcmp(ln, "return") != 0)
	      { strcpy(&label[0], ln);
	        *ln_ = sch;
                if (not d_opt or termstr[0] == '[' or isextern and x_opt)
                { if (u_opt)
                  { int ix;
                    for (ix = -1; label[++ix] != 0; )
                      label[ix] = toupper(label[ix]);
                  }
	          printf(format, label, eny, ipline);
	        }
	        else
	        { strcpy(&labelline[0], ipline);
	          wantch = '{';
	        }
	      }
	      *ln_ = sch;
            }
          }
	  ln = ln__;
        }
      }
    }
  }}}
  fclose(chan);
}


static FILE * user_open_read(fn)
	Char * fn;
{ FILE * chan_ = fopen(fn, "r");
  if (chan_ == null)
    printf("Cannot open %s\n", fn);

  return chan_;
}


private Cc do_all(chan)
	 FILE *  chan;
{ Char * ln = fgets(&fnline[0], FILENAMESZ, chan);
  if (ln == null)
    return ~OK;
  ln[strlen(ln)-1] = 0;
	      
{ FILE * chan_ = user_open_read(ln);
  if (chan_ != null)
    do_it(ln, chan_);
  return OK;
}}  

void main(argc, argv)
        Short   argc;
        Char ** argv;
{ Bool no_files = true;
  Bool need_p = false;
  Short argsleft = argc - 1;
  Char ** argv_ = &argv[1];
  
  for (; argsleft > 0; --argsleft)
  { if      (argv_[0][0] == '-')
    { Char * s = &argv_[0][0];

      while (*++s != 0)
      { if      (*s == 'a')
          termstr = "[[]";
        else if (*s == 'd')
          d_opt = true;
        else if (*s == 'e')
          e_opt = true;
        else if (*s == 'p')
          need_p = true;
        else if (*s == 'q')
          q_opt = true;
        else if (*s == 'r')
          r_opt = true;
        else if (*s == 's')
          s_opt = true;
        else if (*s == 'u')
          u_opt = true;
        else if (*s == 'v')
          v_opt = true;
        else if (*s == 'x')
          x_opt = true;
        else if (*s == '@')
          at_opt = true;
        else if (isdigit(*s))
        { if (nest_lvl == 0)
            nest_lvl = atoi(s);
        }
        else
        { explain();
          exit(0);
        }
      }
    }
    else if (need_p)
    { need_p = false;
      p_opt = true;
      pname = argv_[0];
    }
    else
    { Char * fn = argv_[0];
      Char * eny = fn;
      int trash;

      no_files = false;
#if S_MSDOS
      msd_init(fn, null, MSD_REPEAT | MSD_NOCHD);

      while ((eny = msd_nfile(&trash)) != null)
#endif
      { FILE * chan = user_open_read(eny);
        if (chan != null)
	{ while (true)
	  { if (not at_opt)
	    { do_it(eny, chan);
	      break;
	    }
	    else
	    { Cc cc = do_all(chan);
	      if (cc != OK)
	        break;
	    }
	  }
        }
      }
    }

    ++argv_;
  }
  
  if (no_files)
    if (argc == 1)
      explain();
    else
      while (do_all(stdin) == OK)
        ;
}
