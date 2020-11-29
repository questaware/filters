#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "build.h"


#define SHO_STATE 1


#define M_CASE 1
#define M_KAR  2	/* bastards */


#define MAXLN 1024
#define MAXINDENT 80

static char ipline[MAXLN+4+MAXINDENT];

private void explain()
  
{ fputs( 
	"clay {-#} {-c#} {-p#}filename*\n"
	"   -#   indent interval\n"
	"   -#   (literally #) also indent lines starting #\n"
	"   -c#  comment offset\n"
	"   -p#  parameter offset\n"
	"   -s#  switch arm offset\n"
	"   -t#  tab width in\n"
	"   -T#  tab width in and out\n"
	"   -e   expand tabs\n"
	"   -v   keep vertical spacing\n"
	"   -k   consort with the devil\n"
	"   -w#  page width\n",
         stderr);
  exit(0);
}


Vint line_width = 80;

Vint indent_sz = 2;
Vint cmt_offs = 40;
Vint param_offs = 6;
Vint swarm_offs = 7;
Vint tab_width_in = 8;
Vint tab_width_out = 8;
Vint tab_width_in_is_power_of_2 = true;
Bool expand_tabs = false;
Bool k_r_opt = false;
Bool v_opt = false;
Bool hash_opt = false;

Vint dyn_p_offs;		/* distance from first non-space to lparen */
Vint dyn_p_ind; 		/* indent at start of line for above */
Vint rel_offs;			/* relative offset between two lines */

Bool sho_state = false;

Set16 in_cmt = 0;
#define M_AST 1
#define M_SL  2
#define M_STR 4
#define M_CHR 8


Vint parens = 0;	/* parenthesis nesting level */
Vint equs = 0;	    /* # of = we are on the right hand side of */

Vint nest = 0;
#define indent (nest-noinc[nest])
Vint swindent = 0;	/* # switches in progress */

#define MAXSTK 1000

Vint noinc[MAXSTK];	/* number of false increments at this level */
					/*[preincrement of nest]*/
Vint swtch[MAXSTK];	/* # switches in to bracket ; [preincrement of nest]*/

Vint postp[MAXSTK];	/* # post paren increments */


Bool in_noinc_ctx = false;		/* in no increment nest context */
Vint in_postp_ctx = 0;			/* # post right paren contexts */
Bool ind_nxt_stmt = false;		/* indent the next stmt */



Vint hashoffs[16];			/* queue of offsets in previous lines */
Vint cmtoffs[16];

Vint offslen = 0;
Vint offsix = 0;



Vint skipped_spaces;				/* secondary result */

Char * skipspaces(Char * str)
  
{ skipped_spaces = 0;
  
  for ( ; *str != 0 and isspace(*str); ++str)
  { skipped_spaces += 1;
    if	    (*str == '\t')
    { skipped_spaces += (tab_width_in - 1);
      if (tab_width_in_is_power_of_2)
	skipped_spaces &= ~(tab_width_in-1);
      else
      { skipped_spaces /= tab_width_in;
	skipped_spaces *= tab_width_in;
      }
    }
  }
  return str;
}



Vint lboffs;				/* secondary result */
char * lbstr;				/* tertiary result */
char   lbstrch;
					/* offset of an lb only at eol */

Set16 scan_line(str)
  Char * str;
{ Char * str_  = str;
  Char * la;
  Vint indent_ = indent;
  Vint dyn_poffs = -1;
  Vint lbraceoffs = 0;
  Set16 res = 0;
  Bool this_line_postp_ctx = false;		/* post ')' on this line */
  static Bool define_continues = false;
  
  lbstr = null;

  if (*str == '#' or define_continues)
  { if (define_continues or strncmp("#define", str, 7) == 0)
      define_continues = str[strlen(str)-1] == '\\';
    else
      define_continues = false;
  }
  else
    for ( --str; *++str != 0; )
    { 
      if      (in_cmt & (M_CHR+M_STR))
      { if (*str == '\\' && str[1] != 0)
          ++str;
        else
        { if ((in_cmt & M_CHR) and *str == '\'')
            in_cmt = 0;
          if ((in_cmt & M_STR) and *str == '"')
            in_cmt = 0;
        }
      }
      else if (*str == '/' and str[1] == '*' and (in_cmt & M_SL) == 0)
	in_cmt |= M_AST;
      else if (*str == '/' and str[1] == '/')
	in_cmt |= M_SL;
      else if (in_cmt & (M_AST+M_SL)) 
      { if (*str == '*' and str[1] == '/')
	  in_cmt &= ~M_AST;
      }
      else if (*str == '"')
      { in_cmt |= M_STR;
        lbraceoffs = 0;
      }
      else if (*str == '\'')
      { in_cmt |= M_CHR;
        lbraceoffs = 0;
      }
      else if (in_cmt or isspace(*str))
        ;
      else if (*str == '{')
      { if (str != str_ && !k_r_opt)
	{ lbstr = str;
	  lbstrch = '{';
	  lbstr[0] = 0;
	  break;
	}
	
        nest += 1;
	noinc[nest] = noinc[nest-1] + in_noinc_ctx;
	if (in_postp_ctx > postp[nest-1])
	  in_postp_ctx -= 1;
	postp[nest] = in_postp_ctx;
	parens = 0;
	equs = 0;
	swtch[nest] = -1;
	lbraceoffs = str - str_;
	ind_nxt_stmt = false;
      }
      else
      { lbraceoffs = 0;
	
	if (((strncmp(str, "case", 4) == 0 or strncmp(str, "when", 4) == 0) and 
	     (str[4] == 0 or not isalpha(str[4])) or
	      strncmp(str, "default:", 8) == 0)     and
	      (str == str_ or not isalpha(str[-1])))
	{ in_postp_ctx = postp[nest-1];
	  in_noinc_ctx = false;
	  str += 3;
	  if (swtch[nest] < 0)
	  { swtch[nest] = swindent;
	    swindent += 1;
					/* printf("INCING"); */
	  }
	  res |= M_CASE;
					/* printf("Hincing"); */
	}
	else if (*str == '}')
	{ if (swtch[nest] >= 0)
	    swindent = swtch[nest];
	  nest -= 1;
	  if (nest < 0)
	    nest = 0;
	  in_postp_ctx = postp[nest];
	  in_noinc_ctx = true;
	  parens = 0;
	  equs = 0;
	  for (la = str; *++la != 0 && isspace(*la); )
	    ;
	  if (strncmp(la, "else", 4) == 0)
	  { lbstr = la;
	    lbstrch = *lbstr;
	    lbstr[0] = 0;
	    break;
	  }
	}
	else if (strncmp(str, "else", 4) == 0 and
		(str[4] == 0 or not isalpha(str[4])) and
		(str == str_ or not isalpha(str[-1])) )
	{ in_noinc_ctx = false;
	  str += 3;
	/*in_postp_ctx = postp[nest];*/
	  ind_nxt_stmt = true;
	}
	else if (*str == ';' and nest > 0)
		 /*(this_line_postp_ctx or nest > 0)*/
	{ in_noinc_ctx = true;
	  if (parens == 0)
	    in_postp_ctx -= 1;
	  if (in_postp_ctx < postp[nest])
	    in_postp_ctx = postp[nest];
	 
	  equs = 0;
	  ind_nxt_stmt = false;
	  this_line_postp_ctx = false;
	}
	else if (*str == '(')
	{ if (dyn_poffs < 0 and parens == 0)
	  { dyn_poffs = str - str_;
	    dyn_p_offs = dyn_poffs; 
	    dyn_p_ind = indent_;
	  }
	  parens += 1;
	}
	else if (*str == ')')
	{ in_noinc_ctx = false;
	  
	  if (parens + 1 == equs)
	    equs = 0;
	  parens -= 1;
	  if	  (parens < 0)
	    parens = 0;
	  else if (parens == 0)
	    this_line_postp_ctx = true;
	}
	else if (*str == '=')
	{ Char ch = str[-1];
	  if		(str[1] == '=')
	    ++str;
	  else if (str != str_ and (ch == '!' or ch == '>' or ch == '<'))
	    ;
	  else
	  { if (equs == 0)
	      equs = parens + 1;
	    else
	      equs += 1;
	  }
	}
	else if (*str == '\'')
	{ if		(str[1] == '\\')
	    ++str;
	  if (str[1] != 0 and str[2] != 0)
	    str+=2;
	}
	else
	{ in_noinc_ctx = false;
	}
      }
    }
  
  in_postp_ctx += this_line_postp_ctx;
  lboffs = lbraceoffs;
  in_cmt &= ~(M_SL+M_STR+M_CHR);
  return res;
}


void putspaces(n)
  Vint n;
{ Vint ct = expand_tabs ? 0 : n / tab_width_out;
  n -= ct * tab_width_out;
  while (--ct >= 0)
    fputc('\t', stdout);
  
  while (--n >= 0)
    fputc(' ', stdout);
}

void main(argc, argv)
  short   argc;
  Char ** argv;
{ Bool didfile = false;
  
  Int argsleft = argc - 1;
  Char ** argv_ = &argv[0];
  
  for ( ; ; --argsleft)
  { ++argv_;
    
    if	    (argsleft <= 0)
    { if (didfile)
	break;
    }
    else if (argv_[0][0] == '-')  
    { if      (in_range(argv_[0][1], '0', '9'))
      { indent_sz = atoi(&argv_[0][1]);
	continue;
      } 
      else if (argv_[0][1] == 'e')
      { expand_tabs = true;
	continue;
      }
      else if (argv_[0][1] == 'k')
      { k_r_opt = true;
	continue;
      }
      else if (argv_[0][1] == 'v')
      { v_opt = true;
	continue;
      }
      else if (argv_[0][1] == '#')
      { hash_opt = true;
	continue;
      }
      else if (argv_[0][1] == 'c' or 
	       argv_[0][1] == 'p' or
	       argv_[0][1] == 's' or
	       argv_[0][1] == 't' or
	       argv_[0][1] == 'T' or
	       argv_[0][1] == 'w')
      { Char wh = argv_[0][1];
	Char * nol = &argv_[0][2];
	if (*nol == 0)
	{ --argsleft;
	  nol = *++argv_;
	}
	
	if	(wh == 'c')
	  cmt_offs = atoi(nol);
	else if (wh == 'p')
	  param_offs = atoi(&argv_[0][2]);
	else if (wh == 's')
	  swarm_offs = atoi(&argv_[0][2]);
	else if (wh == 't')
	  tab_width_in = atoi(&argv_[0][2]);
	else if (wh == 'T')
	{ tab_width_in = atoi(&argv_[0][2]);
	  tab_width_out = tab_width_in;
	}
	else 
	  line_width = atoi(&argv_[0][2]);
	continue;
      }
      else if (argv_[0][1] == 'v')
      { sho_state = 1;
	continue;
      } 
      else if (argv_[0][1] == 0)
	;
      else
      { explain();
      }
    }
    tab_width_in_is_power_of_2 = (one_bit(tab_width_in) == tab_width_in);
    
    didfile = true;
  { Char * fn = argv_[0];
    if (argsleft <= 0)
      fn = "-";
    
  { Bool did_op = false;
    Char did_op_stt;
    Vint last_coffs = -1;
    Vint last_indent = 0;
    FILE * chan = strcmp(fn, "-") == 0 ? stdin : fopen(fn, "r");
    char * recycle = null;
    
    if (chan == null)
    { fputs("Cannot open file ", stderr);
      fputs(fn, stderr);
      fputs("\n", stderr);
      continue;
    }
    
    while (true)
    { int slen;
      Char * ln_;
      if (recycle == null)
      { ln_ = fgets(&ipline[4], MAXLN, chan);
        if (ln_ == null)
          break;
same_a:
        slen = strlen(ln_);
        ln_[slen-1] = 0;
        if (!hash_opt && ln_[0] == '#')
        {  
          if (did_op)
	    fputs("\n", stdout);
          fputs(ln_, stdout);
          did_op = true;
          continue;
        }
      }
      else
      { ln_ = recycle;
        recycle = null;
        ln_[0] = lbstrch;
        slen = strlen(ln_)+1;
      }
        
    { Vint prev_ss = skipped_spaces;
      Char * ln = skipspaces(ln_);
      Vint delta = skipped_spaces - prev_ss;
      
      Vint linelen = strlen(ln);
      Vint onest = nest;
      Bool innoi = in_noinc_ctx;
      Vint inpp = in_postp_ctx;
      Bool indns = ind_nxt_stmt;
      Vint oldind = indent + inpp;
      Vint oldparens = parens;
      Vint oldequs = equs;
      Vint olboffs = lboffs;
      Vint oin_cmt = in_cmt;
      Set16 lprops;
      if (!v_opt & ln_[slen-2] == '{' && ln == &ln_[slen-2])
      { strcpy(&ipline[4], "{        ");
        ln_ = fgets(&ipline[4+indent_sz], MAXLN, chan);
        if (ln_ == null)
          break;
        ln_ -= indent_sz;
        goto same_a;
      }
      
      lprops = scan_line(ln);
      recycle = lbstr;
      if (recycle != null)
        recycle[0] = 0;
      

      if (lboffs > 0 and !in_cmt)		/* last left brace at eol */
      { ln[lboffs] = ' ';
	inpp = 0;
      }
      if (ln[0] == '{' and oin_cmt == 0)
      { Vint sskp = skipped_spaces;
	ln = skipspaces(&ln[1]) - indent_sz;
	dyn_p_offs += indent_sz - skipped_spaces - 1;
	skipped_spaces += sskp - indent_sz + 1;
	ln[0]  = '{';
	if (indent_sz > 1)
	  memset(&ln[1], ' ', indent_sz-1);
	indns = 0;
      }
      if (olboffs > 0)			/* prev left brace at eol */
      { ln -= indent_sz;
	ln[0]  = '{';
	if (indent_sz > 1)
	  memset(&ln[1], ' ', indent_sz-1);
	if (oldind > 0)
	  oldind -= 1;
      }
      
    { Bool cmt = ln[0] == '/' and (ln[1] == '*' or ln[1] == '/');
      Bool coffs = in_cmt			   ? skipped_spaces	  :
		   ln[0] == '*' and ln[1] == '/'   ? 
		     last_coffs >= 0		   ? last_coffs 	  :
						     cmt_offs		  :
		   not cmt			   ? 0			  :
		   linelen + cmt_offs > line_width ? 
		     ln == ln_		       ? 0		      :
						 line_width - linelen :
		   last_coffs >= 0		   ? last_coffs 	  :
					     cmt_offs;
      Vint ind = indent + inpp + indns;
      Vint swin = swindent;
      
      if (*ln == '{' and oldind > 0 and (innoi or onest == 0 or inpp > 0))
	oldind -= 1;
      if ((lprops & M_CASE) and swin > 0)
	swin -= 1;
#if SHO_STATE
      if (sho_state)
	printf("[%d %d %d %d %d]", 
	     oldparens, oldequs, rel_offs, last_indent, oldind);
#endif
      if      (oldparens + oldequs == 0)
	rel_offs = 0;
      else if (ln[0] == ')')
	rel_offs =  (dyn_p_ind - indent) * indent_sz + dyn_p_offs;
      else if (rel_offs == 0)
	rel_offs = delta + (last_indent - oldind) * indent_sz;
      else
	rel_offs += delta;
#if SHO_STATE
      if (sho_state)
	printf("%d %d,%d %d,%d %d", 
	     in_cmt, nest, indent, innoi, skipped_spaces, rel_offs);
#endif
      if (*ln == '{' and k_r_opt)
      { fputs("{", stdout);
	++ln;
      }

      if (did_op && (v_opt || did_op_stt != 0 || strncmp(ln, "else", 4) != 0))
	fputs("\n", stdout);

      if (*ln != 0)
        putspaces((in_cmt or cmt ? coffs             : 
		   *ln == '{'    ? oldind*indent_sz  : 
				   ind * indent_sz + rel_offs)
		     +swin * swarm_offs);
      fputs(ln, stdout);
      did_op_stt = *ln;
      did_op = true;
      
      last_coffs = not (cmt or in_cmt) ? -1 : coffs;
      last_indent = oldind;
    }}}
    if (did_op)
      fputs("\n", stdout);
    
    fclose(chan);
    
    if (argsleft <= 0)
      break;
  }}}
}
