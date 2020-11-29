#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
/*#include <limits.h>*/
#include "build.h"


extern Int atol();



#define MAXLN 1024

static char ipline[MAXLN+2];
static char opfname[MAXLN+2];


Char * srcformat = "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld";
Char * tgtformat;
Char * sumformat = null;
Char * expnformat = "v-14+++++++++++++v-13";
/*
Char * srcformat = "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld";
Char * tgtformat = "%ld %s";
Char * sumformat = null;
Char * expnformat = "v1v2+v3v4++v5v6+v7v8+++v9v10+v11v12++v13v14+++v0";
*/

private void explain()

{ fputs( 
  "spread {-d} {-m} {-s srcformat} {-e expn} {-t tgtformat} {-z sumformat} filename*\n"
  "   compute a value, print in rows, accumulate sum\n"
  "    -s  -- scanf ops: !lx,!ld (not !s)\n"
  "                                       DEFAULT 14 integers\n"
  "    -e  -- formula:\n"
  "           v0 is whole line, v-8 is all 8 and v8 the 8th scanf value\n"
  "           p1-9 is the previous outputs (NOT string)\n"
  "           s9 is value 9 up the stack\n"
  "           q999 is literal 999\n"
  "           Operators: + - * / ~(bitnot) #(dup) a(bitand) o(bitor)\n"
  "                                       DEFAULT v-14+++++++++++++v-13\n"
  "    -t  -- printf arguments from the BASE of the stack (!# => skip arg)\n"
  "                                       DEFAULT srcformat\n"
  "    -z  --                             DEFAULT tgtformat\n"
  "    -m  -- use only lines with scanf() > 0\n"
  "    -d  -- echo parameters\n"
  "Example:  -s \"!d !d !d\" -e \"v0v3v1-v2*\" -t \"!x !s\"\n"
  "means        (COL3 - COL1) * COL2 in hex, prepended to i/p\n"
  "  (note %% may replace !)\n",
          stderr);
}



void myabort(Char * msg)

{ fputs(msg,stderr);
  exit(1);
}


#define MAXSTACK 40


#define Q_INT (-1)

Int stack[MAXSTACK];
Vint slens[MAXSTACK];
Vint stix = -1;


void push(Int val, Vint type)

{ ++stix;
  if (stix >= MAXSTACK)
    myabort("Stack overflow\n");
  stack[stix] = val;
  slens[stix] = type;
}



#define can_pop() (stix >= 0)
#define stk_dpth() (stix + 1)

#define op_len() (slens[stix+1])
#define op_len_upstack(n) (slens[stix+1-(n)])

Int pop()

{ if (stix < 0)
    myabort("Stack underflow\n");
  --stix;
  return stack[stix+1];
}


Int upstack(Vint n)

{ if (stix - n + 1 < 0)
    myabort("Stack underflow\n");
  return stack[stix-n+1];
}


Char * skipdigs(Char * src)

{ while (in_range(*src, '0', '9'))
    ++src;
  return src;
}



void set_av(Int * av[], Int v[], Char * fmt)

{ Vint ix = -1;
  Char * s;

  for (ix = MAXSTACK; --ix >= 0; )
    av[ix] = &v[ix];

  for (s = fmt-1; *++s != 0; )
    if (*s == '!' or *s == '%')
    { if (++ix < MAXSTACK)
	if (s[1] == 's' or s[1] == '[')
	{ extern char * calloc();
	  av[ix] = (Int*)calloc(4, 1000);
	/*fprintf(stderr, "string %d at %d\n", ix, av[ix]);*/
	}
    }
}

void main(argc, argv)
	short	argc;
	Char ** argv;
{ if (argc <= 1)
  { explain();
    exit(0);
  }
  
  tgtformat = srcformat;
{ Char * inpr;
  Int tot = 0;
  
  Bool d_opt = false;
  Bool m_opt = false;
  Bool got_all = false;
  Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];
  
  for (; argsleft > 0; --argsleft)
  { if (argv_[0][0] == '-' and argv_[0][1] != 0)
    { if      (argv_[0][1] == 's' or 
	       argv_[0][1] == 't' or 
	       argv_[0][1] == 'z' or
	       argv_[0][1] == 'e')
      { Char wh = argv_[0][1];
	++argv_;
	--argsleft;
      { Char * format = argv_[0];
	if (format[0] == '"')
	  ++format;
	if (format[strlen(format)-1] == '"')
	  format[strlen(format)-1] = 0;      /* risk it !!! */
	if	(wh == 't')
	  tgtformat = format;
	else if (wh == 'z')
	  sumformat = format;
	else if (wh == 's')
	  srcformat = format;
	else
	  expnformat = format;
	if (wh != 'e')
	{ Char * s, * t = format;
	  for (s = format; *s != 0; ++s)
	    *t++ = *s == '!'  ? '%' : 
		   *s != '\\' ? *s  :
		   *++s == 't' ? '\t' :
		   *s	== 'n' ? '\n' : *s;
		 
	  *t = 0;
	}
      }}
      else if (argv_[0][1] == 'd')
      { d_opt = true;
      }
      else if (argv_[0][1] == 'm')
      { m_opt = true;
      }
      else
      { explain();
	exit(0);
      }
    }
    else
    { static Int rowsum[MAXSTACK];
      static Int rowlen[MAXSTACK];
      Char * fn = argv_[0];
      FILE * chan = fn[0] == '-' and fn[1] == 0 ? stdin : fopen(fn, "r");
      
      if (chan == null)
      { fputs("Cannot open file ", stderr);
        fputs(fn, stderr);
        fputs("\n", stderr);
	continue;
      }
      
      for (inpr = expnformat; *inpr != 0; ++inpr)
	if (inpr[0] == 'v' and inpr[1] == '0')
	  got_all = true;
  
      memset(&rowsum[0], 0, sizeof(rowsum));
      memset(&rowlen[0], 0, sizeof(rowlen));
      
    { static Int    v[MAXSTACK];
      static Int   prevv[MAXSTACK];
      static Int * av[MAXSTACK];
      set_av(&av[0], &v[0], srcformat);

      memset(&prevv[0], 0, sizeof(prevv));

      while (true)
      { Char * ln = fgets(&ipline[0], MAXLN, chan);
	if (ln == null)
	  break;
	ln[strlen(ln)-1] = 0;
	
	memset(&v[0], 0, sizeof(v));
	
      { int ct_vals = 
	sscanf(ln, srcformat, av[0], av[1], av[2], av[3], av[4], av[5], av[6],
			      av[7], av[8], av[9],av[10],av[11],av[12],av[13]);
	if (ct_vals <= OK and m_opt)
	  continue;
      /*printf("input: %s\n", ln);**/
      /*printf("fmt %s %d vals\n", srcformat, ct_vals);**/
      /*printf("GOT %lx %lx %lx %lx %lx\n", v[0], v[1], v[2], v[3], v[4]);**/
	for (inpr = expnformat; *inpr != 0; ++inpr)
	{ if	   (*inpr == 'v')
	  { Vint wh = atoi(&inpr[1]);
	    if (wh <= 0)
	      if (inpr[1] != '-')
		push((Int)ln, strlen(ln));
	      else
	      { Vint lim = atoi(&inpr[2]);
		for (wh = -1; ++wh < lim; )
		  push(wh >= ct_vals ? 0 :
		       av[wh] == &v[wh] ? v[wh] : (Int)av[wh], Q_INT);
		inpr += 1;
	      }
	    else
	    { --wh;
	      if (not in_range(wh, 0, MAXSTACK-1))
		myabort("Var oor");
	      push(av[wh] == &v[wh] ? v[wh] : (Int)av[wh], Q_INT);
	    }
	    inpr = skipdigs(&inpr[1])-1;
	  /*fprintf(stderr, "PUSH %d", v[wh-1]);*/
	  }
	  else if (*inpr == 'p')
	  { Vint wh = atoi(&inpr[1]);
	    if (not in_range(wh, 1, MAXSTACK-1))
	      myabort("p0 is illegal");
	    else
	      push(prevv[wh-1], Q_INT);
	    inpr = skipdigs(&inpr[1])-1;
	  }
	  else if (*inpr == '+')
	  { push(pop()+pop(), Q_INT);
	  /*printf("Add");*/
	  }
	  else if (*inpr == 'a')
	    push(pop()&pop(), Q_INT);
	  else if (*inpr == 'o')
	    push(pop()|pop(), Q_INT);
	  else if (*inpr == '-')
	  { Int src = pop();
	    push(pop()-src, Q_INT);
	  }
	  else if (*inpr == '*')
	    push(pop()*pop(), Q_INT);
	  else if (*inpr == '/')
	  { Int denum = pop();
	    push(denum == 0 ? 0 : pop()/denum, Q_INT);
	  }
	  else if (*inpr == '~')
	    push(~pop(), Q_INT);
	  else if (*inpr == '#')
	  { Int val = pop();
	    push(val, op_len());
	    push(val, op_len());
	  } 
	  else if (*inpr == 's')
	  { Vint up = atol(inpr+1);
	    push(upstack(up), op_len_upstack(up));
	    inpr = skipdigs(inpr+1)-1;
	  /*printf("Lit");*/
	  }
	  else if (*inpr == 'q')
	  { push(atol(inpr+1), Q_INT);
	    inpr = skipdigs(inpr+1)-1;
	  /*printf("Lit");*/
	  }
	  else if (in_range(*inpr, '0','9'))
	  { push(atol(inpr), Q_INT);
	    inpr = skipdigs(inpr)-1;
	  /*printf("Lit");*/
	  }  /*
	  else
	    printf("SKIP %c", *inpr);
	  printf("(%lx)", stack[stix]); */
	}
	  
      { Vint ix;
	for (ix = stk_dpth(); --ix >= 0; )
	{ v[ix] = pop();
	{ Vint len = op_len();
       /* printf("Did pop %lx ", v[ix]); */
	  if	  (len == Q_INT)
	    rowsum[ix] += v[ix];
	  else if (rowlen[ix] <= len)
	    rowlen[ix] = len + 1;	    
	}}
	
	/*fputs("\t", stdout);*/
	printf(tgtformat, v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8],
		  v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[16], v[17]);
	fputs("\n", stdout);
	memcpy(&prevv[0], v, sizeof(prevv));
      }}}
      
      memset(ipline, ' ', sizeof(ipline)-1);
      ipline[sizeof(ipline)-1] = 0;
    { Vint i;
      for (i = MAXSTACK; --i >= 0; )
	if (rowlen[i] > 0)
	  rowsum[i] = rowlen[i] >= sizeof(ipline) 
				? "" : &ipline[sizeof(ipline)-rowlen[i]];

      if (sumformat == null)
	sumformat = tgtformat;

      printf(sumformat, 
      rowsum[0], rowsum[1], rowsum[2], rowsum[3],
      rowsum[4], rowsum[5], rowsum[6], rowsum[7],
      rowsum[8], rowsum[9], rowsum[10],rowsum[11],
      rowsum[12],rowsum[13],rowsum[14],rowsum[15], rowsum[16], rowsum[17]
	    );
      printf("\n");

      fclose(chan);
    }}}

    ++argv_;
  }
  if (d_opt)
    printf("SRC:%s\nTGT:%s\n",srcformat,tgtformat);
}}
