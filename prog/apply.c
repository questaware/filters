#include   <stdio.h>
#include   <ctype.h>
#include   <fcntl.h>
#include   "version.h"


extern Char * strchr();

#define MAX_LN 512

extern Char * realloc();
#undef malloc


				/* module op */
static Char * opstr;
static Int opsz;
static Int oplim;

private void append_c(c)
	 Int c;
{ opsz += 1;
  if (opsz >= oplim)
  { oplim += 100;
    opstr = realloc(opstr,oplim);
  }

  opstr[opsz-1] = c;
  opstr[opsz] = 0;
}



private void append_s(s)
	 Char * s;
{ if (s != null)
    while (*s != 0)
      append_c(*s++);
}
				/* end module op */

Char ** argsarr_;
Int argsarrl;

#define PRM_INC 8


void extend_arr()

{ Char ** argsarr__ = (Char**)malloc((argsarrl+PRM_INC)*sizeof(Char*));
  fast Int i;
  for (i = 0; i < PRM_INC; ++i)
  { argsarr__[argsarrl+i] = malloc(MAX_LN+1);
    argsarr__[argsarrl+i][0] = 0;
  }
  
  if (argsarr_ != null)
  { for (i = 0; i < argsarrl; ++i)
      argsarr__[i] = argsarr_[i];
    free(argsarr_);
  }
  argsarrl += PRM_INC;
  argsarr_ = argsarr__;
}



private void explain()

{ fprintf(stderr, "%s%s%s%s%s%s%s%s%s%s",
	"apply {-i} {-n} {-fC} {-N} pattern { file }*\n",
	"  where C is a character or nothing for space\n",
	"  where N is the number of operands in a command\n\n",
	"   -i -- Ignore non zero statuses\n",
	"   -n -- Say what would be done dont do it\n",
	"   -f -- Field separation character\n\n",
	"  where the pattern is delimited by \"\" or ''\n",
	"	   and contains %%1 ... %%9, and/or %%*\n",
	"	  respectively the nth and all operands\n",
	"  where file is an input file or - for stdin\n");
}

void main(argc, argv)
	Int	argc;
	Char ** argv;
{ if (argc <= 1)
  { explain();
    exit(0);
  }
  
{ Int argsleft = argc;
  Char ** argv_ = &argv[1];

  Int bite = 0; 			/* number of args per bite */
  Char fld_sep = '\n';
  Bool n_opt = false;
  Bool i_opt = false;
  
  for (; argsleft > 0; --argsleft)
  { if (argv_[0][0] != '-')
      break;

    if	    (in_range(argv_[0][1], '0', '9'))
    { bite = atol(&argv_[0][1]);
      if (bite > 9)
      { fprintf(stderr, "-N too big\n");
	exit(0);
      }
    }
    else if (argv_[0][1] == 'f')
      fld_sep = argv_[0][2];
    else if (argv_[0][1] == 'n')
      n_opt = true;
    else if (argv_[0][1] == 'i')
      i_opt = true;
    else if (argv_[0][1] != 0)
    { explain();
      exit(0);
    }
 
    ++argv_;
  }

{ Bool morecmds = true;
  Bool rdg = argsleft < 3 || strcmp(argv_[1], "-") == 0;
  Id cmdfd = dup(fileno(stdin));
  FILE * cmdin = cmdfd < 0 or not rdg ? stdin : fdopen(cmdfd, "r");
  Char * cmd = argv_[0];
  Int cmdct = 0;
  Int ct, i;
 
  if (not rdg)
    argsarr_ = &argv_[1];
  else 
  { argsarrl = 0;
    extend_arr();
  { FILE * si = freopen("/dev/tty", "r", stdin);
    if (si == null)
      si = freopen("/dev/null", "r", stdin);
  }}
	
  oplim = strlen(cmd)+200;    /* say */
  opstr = malloc(oplim+1);

  for (ct = rdg ? 0x7fffffff : argsleft - 2; 
       ct > 0; ct -= bite)				/* and not break */
  { Char fst_esc = 0;
    Char sec_esc = 0;	/* in "" or '' */
    Char * s = cmd;
    Int bitesofar = 0;

    opstr[0] = 0;
    opsz = 0;
    
    for (; *s != 0; ++s)
    { Char c = *s;
    
      if (s[1] != 0)
      { if	(fst_esc == 0)
	{ if	  (c == '\'' or c == '"')
	    fst_esc = c;

	  if (c == '%' and (in_range(s[1], '1', '9') or s[1] == '*'))
	  { Bool all = s[1] == '*';
	    Int ix = all ? 1 : s[1] - '0';
	    Int ix_;
	  
	    if (rdg or ix <= ct)
	    { c = 0;				/* suppress the % */
	      ++s;
	      
	      for (ix_ = ix; ix_ > bitesofar and ix_ <= ct; )
	      { if (rdg)
		{ if (fld_sep == '\n')
		  { if (fgets(argsarr_[bitesofar], MAX_LN, cmdin) == null)
		    { ct = -1;			/* set exit condition */
		      break;
		    }
		    else
		      strchr(argsarr_[bitesofar], 0)[-1] = 0; /* remove \n */
		  }
		  else 
		  { Char * t = &argsarr_[bitesofar][0];
		    Int lim = MAX_LN;
		    Short ch;
		    
		    while (true)
		    { ch = getc(cmdin);
		      if (ch == EOF or ch == '\n' or 
			   not isspace(ch) or fld_sep != 0)
			break;
		    }
		      
		    while (true)
		    { if (ch == EOF or ch == '\n' or --lim <= 0  or
			  fld_sep == 0 and isspace(ch) or ch == fld_sep)
		      { *t++ = 0;
			break;
		      }
		      *t++ = ch;
		      ch = getc(cmdin);
		    }
		    if (ch == EOF)
		    { ct = -1;			/* set exit condition */
		      break;
		    }
		  }
		}
		++bitesofar;
		if (bitesofar > bite)
		  bite = bitesofar;
		if (bitesofar >= argsarrl)
		  extend_arr();
		if (all)
		  ++ix_;
	      }
	    { Int lim = all ? bitesofar : ix_;
	      for (ix_ = ix; ix_ <= lim; ++ix_)
	      { append_s(argsarr_[ix_-1]);
		if (all)
		  append_c(' ');
	      }
	    }}	 
	  }
	}
	else 
	{ if	  (sec_esc == c)
	    sec_esc = 0;
	  else if (sec_esc == 0)
	  { if	    (c == fst_esc)
	      fst_esc = 0;
	    else if (c == '\'' or c == '"')
	      sec_esc = c;
	  }
	}
      }
      if (c != 0)
	append_c(c);
    }
    append_c(0);
    if (bitesofar != 0 or cmdct == 0)
    { if (n_opt)
      { fputs(opstr, stdout);
	putc('\n', stdout);
      }
      else
      { if (morecmds)
	{ Cc cc = system(opstr);
	  if (cc != 0 and not i_opt)
	    morecmds = false;
	}
      }
      ++cmdct;
    }
    if (bitesofar == 0)
      break;

    if	    (not rdg)
      argsarr_ += bite;
    else				/* consume extra args */
      while (bitesofar < bite and ct > 0)
      { if (fgets(argsarr_[bitesofar], MAX_LN, cmdin) == null)
	  ct = -1;
	
	++bitesofar;
	if (bitesofar >= argsarrl)
	  extend_arr();
      }
  }
}}}
