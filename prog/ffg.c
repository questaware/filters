#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#ifndef S_IREAD
# define S_IREAD S_IRUSR
#endif

#ifndef S_IFMT
# define S_IFMT _S_IFMT
#endif

#ifndef S_IFDIR
# define S_IFDIR _S_IFDIR
#endif

#ifndef S_IFLNK
# define S_IFLNK _S_IFLNK
#endif

#include "build.h"
#if S_MSDOS
#include <io.h>
#define msd_isslink() 0
#define O_M_IC 4
#include <io.h>
#else
#include <unistd.h>
#define O_M_IC 0
#endif
#include "../common/h/args.h"
#include "../common/h/msdir.h"
#include "../common/h/msera.h"
#include "../common/h/eprintf.h"
#include "../util/getline.h"
#include "../util/grutil.h"

#ifndef S_IREAD
# define S_IREAD S_IRUSR
#endif
extern Int asc_to_tad(const char * s);
extern Char * tad_to_asc(Nat4 date);
extern Char * flook(char wh, const char * str);

extern Short grabchar();				/* from fip.c */
extern FILE * splitip();				/* from fip.c */
extern void serialclose();


#define M_VERBOSE 1
#define M_SKIPSP  2
#define M_IFTHERE 4			/* also defined in diffs.c */


#define NO_FMT 19

#define M_AND     1  /* must be 1 */
#define M_NOT     2
#define M_IC      4
#define M_MATCHWORD 8
#define M_DIRNM  16
#define M_MARK   32
#define M_SIMPLE 64

#if M_DIRNM != MSD_DIRY
  error error
#endif

typedef struct Patcell_s
{ struct Patcell_s * next;
  Byte		     props;
  Char		     c[1];
} Patcell_t, *Patcell;

#define IX_OR	0
#define IX_AND	1  /* must be 1 */
#define IX_ORN	2
#define IX_ANDN 3

  Byte props[4] = {0};

  Bool split_needed = false;
  Vint a_opt = false;
  Vint at_opt = 0;
  Char * at_fn = NULL;
  Bool i_opt = false;
  Bool d_opt = false;
  Bool nosl_opt = false;
  Bool q_opt = false;
  Bool qq_opt = false;
  int  v_opt = 0;
  Bool vv_opt = false;
  Bool u_opt = false;
  Vint long_opt = 0;
  Bool ct_opt = false;
  Short prepost_opts = 0;
  Bool fn_or = false;		/* logical or of filename patterns */
  Vint rec_clamp = 100;
  Char	 e_fmt_kind[NO_FMT];
  Char * e_fmt[NO_FMT] = { "" };
  Short e_fmt_ix = 0;
  Patcell gr_pat = null;
  char * gr_first = null;
  Vint gr_and = 0;		/* count of gr and's */
  Patcell re_exp = null;
  Vint re_and = 0;		/* count of re and's */
  Int s_lo_opt = 0;
  Int s_hi_opt = 0x7fffffff;
  Nat4 date_e_opt = 0;
  Nat4 date_l_opt = 0xffffffff;
  Int g_filter = 0; 
  Int g_one_suff = 0;
  Char * cmp_dir = null;
  Char sep_ch = '/';
  Vint verb_comp = 0;
  Vint file_ct = 0;
  Int  file_tot_sz_lo = 0;
  Int  file_tot_sz_k = 0;
  Vint same_ct = 0;
  Vint diff_ct = 0;

  Vint glob_len = 28;
  Bool pending_delete = false;

  Set16 want_diry;			/* a true global variable */
  Set16 msd_simple = MSD_SIMPLE;	/* avoid stats on files */
  
  Vint exc_opt = 0;         /* -ve ignore case, 1: start of line, else 2 */
  Char exc_str[120];

Char fnbuff[512];
Char cmd_line[1024];


char * invokenm;

FILE * stdip;



static void explain(void)

{
#if S_MSDOS == 0 || 1
	const char help[] = 
#include "helptxt.h"
		;
	fputsout(help);
#else
{	static const char helpfn[] = "Cannot find ffghlp.txt";
	Char * afn = flook('p', &helpfn[12]);
	if (afn == null)
		fputsout(helpfn);
	else
	{ int ip = open(afn, O_RDONLY);
		if (ip > 0)
		{ Vint ct;
			while ((ct = read(ip, &cmd_line[0], sizeof(cmd_line))) > 0)
				write(1, cmd_line, ct);
		}
	}
}
#endif
  exit(0);
}



void myexit(int n)

{
#if S_MSDOS == 0
  serialclose();
#endif
  exit(n);
}

void eprintdet()

{ extern struct tm time_dcr;    /* from tad.c */
	#if S_MSDOS
			  if (at_opt)
				{ struct tm * res = gmtime(&msd_stat.st_mtime);
				  if (res != 0)
				    memcpy(&time_dcr, res, sizeof (struct tm));
				}
				else
					(void)tad_to_asc(msd_stat.st_mtime);
	#else
			{ struct tm * res = localtime(&msd_stat.st_mtime);
			  if (res != 0)
	    		memcpy(&time_dcr, res, sizeof (struct tm));
			}
	#endif
				eprintf(null, "%02d/%02d/%02d %2d:%02d ",
				       (short)time_dcr.tm_year %100,
			  	     (short)time_dcr.tm_mon+1,
			    	   (short)time_dcr.tm_mday,
			      	 (short)time_dcr.tm_hour,
			      	 (short)time_dcr.tm_min,
			       	 (short)time_dcr.tm_sec
	       			);
/*	    eprintf(null, "%ld ", msd_stat.st_mtime); */

				if (long_opt > 1)
#if S_MSDOS
					eprintf(null, "%c%c%c%c ",
						       msd_attrs & MSD_ROFILE  ? ' ' : 'w',
						       msd_attrs & MSD_CHGD    ? 'a' : ' ',
						       msd_attrs & MSD_HIDFILE ? 'h' : ' ',
						       msd_attrs & MSD_SYSFILE ? 's' : ' '
						     );
#else
					eprintf(null, "%3o ", msd_stat.st_mode & 0777);
#endif
}

static Cc mygets0(Char * tgt_, Vint maxlen)

{ Char * tgt = tgt_;
  fast Vint ct;

  while ((ct = read(fileno(stdip), tgt, 1)) > 0 && *tgt > 'J'-'@' && --maxlen > 0)
  { if (*tgt != 'M'-'@')
      ++tgt;
  }

  *tgt = 0;
  return ct <= 0 && tgt == tgt_ ? HALTED : OK;
}



void add_cell(Patcell * list_ref,
							Char *	  str_,
							Byte	  props)
{ int q = 0;
  char * str = str_;
  int len = strlen(str);

  if (str == null)
    explain();

	if (gr_first == NULL)
		gr_first = str;

#if S_MSDOS
  if (list_ref == &gr_pat)
  { char * t;
    for (t = str_; *++t != 0; )
      if (*(t-1) == '[' && *t == '!')
        *t = '^';
  }
#endif

{ Patcell pcell = (Patcell)malloc(sizeof(Patcell_t)+len);
  if (pcell != null)
  { pcell->next  = *list_ref;
    *list_ref = pcell;
#if 0
    if (*str == '\'')
    { ++str;
      --len;
      q = 1;
    }
#endif
    strcpy(&pcell->c[0], str);
//  if (q && len > 0)
//    pcell->c[--len] = 0;

    while ((props & M_MATCHWORD) && len > 0 && isspace(pcell->c[len-1]))
      pcell->c[--len] = 0;
      
  /*eprintf(null, "W %s W\n", str_);*/
  /*eprintf(null, "X %s X\n", pcell->c);*/

    pcell->props = props;
  { Char * s; /* = str;
    if (str[0] == '*')
      ++s;
    for (; *s != 0; ++s)
      if (*s == '*' || *s == '?')
        pcell->props |= M_SIMPLE;		** not in use */
        ;

    if (pcell->props & M_IC)
      for (s = pcell->c; *s != 0; ++s)
        *s = toupper(*s);
    if (len > 0 && str[len-1] == '/')
      pcell->props |= M_DIRNM;
  }}
}}



static void fmt_cmd(
    int    ix,
    char * eny)
{ int tl = strlen(eny);
  while (--tl >= 0 && eny[tl] != '/' && eny[tl] != '\\')
    ;
  ++tl;
  
{ char * incs[5];
  int    frees[5];
  int incix = -1;
  int fix;
  int fmtl = strlen(e_fmt[ix]);
  
  for (fix = upper_index(frees)+1; --fix >= 0; )
    frees[fix] = 0;
  
  for (fix = -1; ++fix < fmtl; )
    if (e_fmt[ix][fix] == '%')
    { ++incix;
      if (incix <= upper_index(incs))
      { incs[incix] = eny;
	if	(e_fmt[ix][fix+1] == 't')
	  incs[incix] += tl;
	else if (e_fmt[ix][fix+1] == 'b')
	{ int tl_;
	  Int sl = strlen(eny+tl);
	  char * beny = malloc(eny);
	  strcpy(beny, &eny[tl]);
	  incs[incix] = beny;
	  frees[incix] = 1;
	  for (tl_ = sl; --tl_ >= 0; )
	    if (beny[tl_] == '.')
	      beny[tl_] = 0;
	}
      }
    }
  
  spare_sprintf_arg = msd_stat.st_ino;
  eprintf(&cmd_line[0], e_fmt[ix], incs[0], incs[1], incs[2], incs[3]);
  for (fix = upper_index(frees)+1; --fix >= 0; )
    if (frees[fix])
      free(incs[fix]);
}}

#define M_DOPROC 1
#define M_DOPUSH MSD_DIRY


static Set16 want_file(char * eny)

{ 
  Set16 is_diry = msd_attrs & (MSD_DIRY+MSD_POST);
  want_diry = is_diry & prepost_opts;

  if (re_exp != null) 
       /* fn_or | and_res** || is_diry && not d_opt **dirys are explicitly selected*/
  { Vint inneed = 0;
    Vint no_ct = 0;
    Patcell pc;
    for (pc = re_exp; pc != null;  pc = pc->next)
    { 
			char ch = *match_fn_re_ic(&eny[0], pc->c, (pc->props& M_IC));       
      if      (ch == 0
             &&((pc->props ^ is_diry) & MSD_DIRY) ==0 )
      {
        if (pc->props & M_NOT)
				  return 0;
	
				inneed = 1;
      }
      else if (ch == '/' && !(pc->props & M_NOT))
      	return M_DOPROC+M_DOPUSH+MSD_MATCHED;
      else
      { if (!(pc->props & M_NOT))
				  no_ct += 1;
      }
    }
      
    if (inneed > 0 || no_ct == 0)
    { 
      /*eprintf(null, "NEEDED %s\n", &eny[0]);*/
      /*want_diry |= (is_diry & MSD_DIRY);*/
    }
    else 
    { if (! is_diry)
        return 0;
      if (want_diry == 0 && (is_diry & MSD_POST) && ! d_opt)
        return 0;
    }
  }
  else 
    if (want_diry == 0 && (is_diry & MSD_POST) && ! d_opt)
      return 0;
  
#if 0
  eprintf(null, "SSSS %d %ld %ld\n", is_diry, s_lo_opt, msd_stat.st_size);
  eprintf(null, "CCCC %lx %lx\n", msd_stat.st_mtime, date_e_opt);
#endif
  if (is_diry)
    return want_diry ? M_DOPROC+M_DOPUSH : M_DOPUSH;
    
  if ((msd_simple & MSD_SIMPLE) == 0)
  { if ((unsigned long)msd_stat.st_size < (unsigned long)s_lo_opt ||
        (unsigned long)msd_stat.st_size > (unsigned long)s_hi_opt)
      return 0;
/*  if (date_e_opt != 0)
    { eprintf(null, "File time %s %lx %lx\n", eny, msd_stat.st_mtime, date_e_opt);
      eprintf(null, "    %x\n",    date_e_opt-msd_stat.st_mtime);
    }*/
    if (date_e_opt > 0 && (unsigned long)msd_stat.st_mtime <= date_e_opt ||
                           (unsigned long)msd_stat.st_mtime >= date_l_opt)
       return 0;
  }
  return M_DOPROC;
}


static void doit(
	Set16		pset,
	char *	diry,
	char *	eny,
	Vint	dirlen)

{      Char * sheny = &eny[dirlen];
  fast Char ch;
  Set16 is_diry = msd_attrs & (MSD_DIRY+MSD_POST);
  
  if (sheny[0] == '.' && sheny[1] == '/')
    sheny += 2;
    
  if (qq_opt)
  { char * t = sheny;
    while (*++t != 0)
      if (*t == '/')
        sheny = t+1;
  }

/*printf("Doit: %s %s\n", diry, eny);*/

  if ((i_opt || d_opt == true) && (! is_diry ||want_diry ||(pset & MSD_MATCHED)))
  { if (split_needed && d_opt == true || i_opt == 1)
    { stdip = splitip();
      split_needed = false;
    }
    if (d_opt == true)
    { d_opt = 2;
      msg_str("Press d to delete files", "");
      if ((grabchar() | 0x20) != 'd')
				myexit(0);
    }
    if (i_opt == 1)
    { fputserr("Y, y, o, CR confirms one, SP all, ^A-Z quits\r\n");
      i_opt = 2;
    }
    if (i_opt)
    { fmt_cmd(0, sheny);
      eprintf((char*)stderr, "%s ? ", e_fmt[0][0] != 0 ? cmd_line : sheny);

      ch = (char)grabchar();
      fputserr("\r\n");
      if      (ch == ' ')
				i_opt = false;
      else if (ch == A_CR || ch == A_LF ||
	       ch == 'Y'  || ch == 'y'	/*or ch == 'o'*/)
				;
      else if (ch <= 'Z' - '@')
      { myexit(0);
      }
      else
				return;
    }
  }
  
{ Vint leny_ = strlen(eny);
	Vint occ_ct = 0;
  Cc cc = OK;

  if (is_diry)
    cc = want_diry || (pset & MSD_MATCHED) ? OK : NOT_FOUND;
  else 
  { file_ct += 1;
    file_tot_sz_lo += msd_stat.st_size;
    file_tot_sz_k += file_tot_sz_lo >> 10;
    file_tot_sz_lo &= 0x3ff;
    if (cmp_dir != null)
    { Vint leny;
      for (leny = -1; (ch = eny[++leny]) != 0 &&
		       ch == diry[leny]; )
        ;
      if (leny > 0 && ch == sep_ch)
        ++leny;
      
    { Vint cmp_dirl = strlen(cmp_dir);
      Vint cmp_dirl_ = cmp_dirl;
      Vint tln = leny_ - leny + cmp_dirl_ + 3;
      Char * cmp_file = (char*)malloc(tln);
      if (cmp_file == null)
      { fputserr("Out of Mem\n");
        myexit(0);
      }
      else
      { strcpy(&cmp_file[0], cmp_dir);
        ch = cmp_file[cmp_dirl_-1];
        if (ch != '/' &&
				    ch != '\\' &&
				    ch != ':')
				  cmp_file[cmp_dirl_++] = sep_ch;
        strcpy(&cmp_file[cmp_dirl_], &eny[leny]);
#if 0
        eprintf((char*)stderr, "Comparing %s %s\n", sheny, cmp_file);
#endif
#if 0
        if (strlen(cmp_file) >= tln)
				  fputserr("INT Err");
#endif
      { extern Cc same_files(char *, char *, Vint);
        Cc dc = same_files(eny, cmp_file, verb_comp);
        free(cmp_file);
        if      (dc == OK)
        { if (v_opt)
				    msg_str("Same ", sheny);
				  same_ct += 1;
				  return;
        }
        else 
        { if      (dc == 1)
				  { diff_ct += 1;
          }
          else if (dc == -1)
          { eprintf(null, "Cannot read file %s\n", eny);
            return;
          }
          else
          { if (eny[0] == '.' && (eny[1] == '/' || eny[1] == '\\'))
              eny += 2;
          { Char * t = eny-1;
            while (*++t != 0)
              if (in_range(*t, 'a', 'z') || in_range(*t, 'A', 'Z'))
                *t ^= 0x20;

            if (e_fmt_ix == 0  && (verb_comp & M_IFTHERE) == 0)
              eprintf(null, ">> %s\n", eny);
            return;
          }}
        }
      }}
    }}
    if (gr_pat != null)
    { Patcell pcell;
      Bool or_res = false;
      Vint and_res = gr_and > 0;

      static Lbuf_t linebuf;
      char * bn;
      if (at_opt && diry[0] == '.' && diry[1] == 0)
        bn = eny-1;
      else
      {
#if S_MSDOS == 0 | S_WIN32
      bn = strrchr(eny, '/');
      if (bn == null)
#endif
        bn = eny-1;
      }
    /*printf("Grep: %s\n", bn+1);*/
    { FILE * ip = fopen(bn+1, "r");
      if (ip == null)
      { if (vv_opt)
				{ char cwdb[160];
          eprintf(null, "CWD %s Cannot read %s\n", getcwd(cwdb, 159), bn+1);
          eprintf((char*)stderr, "Cannot read %s\n", bn+1);
        }
        cc = HALTED;
      }
      else
      { char blank[31];
        char filename[132];
        strpcpy(filename,sheny, sizeof(filename));
        if (g_one_suff)
        { cc = strlen(filename);
          while (--cc > 0 && filename[cc] != '.')
            ;
          if (cc > 0) filename[cc] = 0;
        }
        memset(blank,' ',30);
        blank[30] = 0;

        /*eprintf(null, "GREPF\n");*/
      
        for (pcell = gr_pat;	pcell != null;  pcell = pcell->next)
				  pcell->props &= ~M_MARK;

        while (v_opt || ct_opt || occ_ct == 0)
        { Char * ln = ffgetline(&linebuf, ip);	/* For each line */
				  if (ln == null)
				    break;

				  for (pcell = gr_pat; pcell != null;  pcell = pcell->next)
				  { if ((pcell->props & M_AND) && and_res < 0)
				      and_res = true;
				    cc = grep_line(ln, pcell->c, (Byte)(pcell->props&~(M_MARK+M_SIMPLE)));
				    if (cc == OK)
				    { pcell->props |= M_MARK;
				      if (pcell->props & M_NOT)
				      { if (pcell->props & M_AND) 
				          and_res = false;
				      }
				      else
				      { if ((pcell->props & M_AND) == 0)
				        { occ_ct += 1;
								  or_res = true;
								  if (v_opt)
								  { while (*ln == ' ' || *ln == 9)
								      ++ln;
								    eprintf(null, "%s:	%s\n", filename, ln);
								    if (v_opt > 1)
								    { int sp = strlen(filename);
								      if (sp > 30) sp = 30;
								      cc = v_opt;
								      while (--cc > 0)
								      { Char * ln = ffgetline(&linebuf, ip);
							          if (ln == null)
							            break;
								        while (*ln == ' ' || *ln == 9)
								          ++ln;
								        eprintf(null, "%s:	%s\n", &blank[30-sp], ln);
								      }
								    }
								  }
								  break;
								}
				      }
				    }
				  }
				}
				
        fclose(ip);
        cc = occ_ct == 0;
        if (cc != OK)
        { for (pcell = gr_pat; pcell != null;  pcell = pcell->next)
				  { Vint props = pcell->props;
				    if ((props & (M_NOT+M_AND+M_MARK)) == M_NOT)
				      or_res = 1; 	  /* missing => true */
				    if ((props & (M_NOT+M_AND+M_MARK)) == M_AND)
				      and_res = false;	  /* required => false */
				  }
				  if (or_res || and_res)
				  { cc = OK;
				    if (v_opt)
				      msg_str(sheny, "");
				  }
        }
        if (cc == OK)
				  same_ct += 1;
      }
    }}
  }

  if      (cc != OK)
    ;
  else if (d_opt)
  {/*eprintf(null, "DOPT %s %x\n", eny, msd_attrs);*/
    if (msd_attrs & MSD_DIRY)
      ;
    else
    { if (! at_opt && (msd_attrs & MSD_POST))
      { /*eprintf(null, "Pend %s %x\n", eny, msd_attrs);*/
        if (leny_ < sizeof(cmd_line))
				{	strcpy(&cmd_line[0], eny);
        	pending_delete = ! g_filter;
        }
      }
      else
      { if (v_opt) 
          msg_str("Erasing ", eny);
      { char * fn_ = eny-1;
        char * fn = fn_;
#if S_MSDOS == 0
        if ((msd_simple & MSD_NOCHD) == 0)
        { for ( ; *++fn_ != 0; )
            if (*fn_ == '/')
               fn = fn_;
        }
#endif
      { Cc cc = erase_file(fn+1, ' ');
				if (cc != OK)
				{ eprintf(null, "CD %d %s\n", errno, eny);
		    }
      }}}
    }
  }
  else
  { if (sep_ch == '\\')
    { char * t = strchr(sheny, '/');
      while (t != NULL)
      { *t = '\\';
        t = strchr(sheny, '/');
      }
    }
    if (e_fmt_ix != 0)
    { 
      #define ix ch
      for (ix = -1; ++ix < e_fmt_ix; )
      { fmt_cmd(ix, sheny);
				if (e_fmt_kind[ix] == 'p')
				{  msg_str(cmd_line, "");
				}
				else
				{  fflush(stdout);
#if S_MSDOS
				   shellprog(cmd_line);
#else
			 	  system(cmd_line);
#endif
				}
		  }
      #undef ch
    }
    else
    { if (long_opt != 0)
      {	eprintdet();
				fputsout(sheny);
	    }
      else if (gr_pat == null || ! v_opt)
				fputsout(sheny);
      if (ct_opt && occ_ct > 0 && ! v_opt)
      { eprintf(null, "\t: %d", occ_ct);
      }
      if (long_opt != 0)
      { Vint len = strlen(sheny);
				if (len > glob_len)
				  glob_len = len;
				if (glob_len > 54)
				  glob_len = len;
				len = glob_len - len;
				while (--len > 0)
				  fputsout(" ");
				eprintf(null, "\t%9ld", msd_stat.st_size);
      }
      if (gr_pat == null || ! v_opt)
				fputsout("\n");
    }
  }
}}

Char * env_args;
int env_args_len;


Char * get_ee(Bool alloc)

{ if (env_args_len <= 0)
    return next_arg(alloc);
    
  while (*env_args != 0 && isspace(*env_args))
  { ++env_args;
    --env_args_len;
  }
{ Char *ss, * s = env_args;
  Char q = *s == '\'' ? *s : 0;
  if (q)
    ++s;

  while (*s != 0 && (q ? *s != q : !isspace(*s)))
    ++s;

  *s = 0;

  ss = env_args;
  env_args_len -= s+1-ss;
  env_args = s+1;
/*printf("Get_ee >%s<\n", ss);*/
  return ss;
}}



static void process_args()

{ Vint echo = 0;
  Char ch;
  Char * s;

  while ((s = get_ee(false)) != null)
  { if (echo)
      printf("%s\n", s);

    if (*s == '@')
    { char * var = getenv(s+1);
    /*printf("GE %s %s\n", s+1, var);*/
      if (var == NULL)
      { printf("\nArg variable ! found %s\n", s+1);
        explain();
      }
      else
      { env_args = var;
        env_args_len = strlen(var);
      }
      continue;
    }
    if      (*s == '/' && s[1] == '/')
      --s;
    else if (*s != 0 && *s != '-' && *s != '+')
    { if (argix_ > 1 || argc_ <= 2)
      { --argix_;
				break;
      }
      /*printf("Pattern:%s\n", s);*/
      add_cell(&gr_pat, s, false);
      msd_simple |= MSD_CHGD;
      continue;
    }
  /*eprintf(null, "Seeing %d %s", '#', s);*/
  { Vint conj = *s == '+';
    Vint negprops = 0;
    Bool dodiff;

    while ((ch = *++s) != 0)
    { switch (ch)
      { case '.':  echo = 1;
        when '@':  at_opt += 1;
								   split_needed = true;
								   if (s[1] != 0 && s[1] != '@')
								   { at_fn = s + 1;
								     s += strlen(s) - 1;
								   }
				when 'a':  a_opt = true;
				when 'n':  negprops = M_NOT;
				when 'f':{ Vint sargc = argc_;
								   Vint sargix = argix_;
								   argc_ = 0;
								   process_args();
								   argc_ = sargc;
								   argix_ = sargix;
								 }	  
				when 'C':  verb_comp |= M_VERBOSE;
				case 'c':  dodiff = false;
							     for ( ; s[1] != 0; ++s)
								     if      (s[1] == 'b')
								       verb_comp |= M_SKIPSP;
								     else if (s[1] == '?')
								       verb_comp |= M_IFTHERE;
								     else if (s[1] == 'v')
								       dodiff = true;

								   cmp_dir = get_ee(true);
								   if (cmp_dir == null)
								     explain();

								   if (dodiff)
								   { Char buf[272];
								     sprintf(buf,"diff -i -a -b %s%s%%s %%s",
								                 cmp_dir,
								                 cmp_dir[strlen(cmp_dir) -1] == '/' ? "" : "/"
								            );
								     e_fmt_kind[e_fmt_ix] = 'p';
								     e_fmt[e_fmt_ix++] = strdup(buf);
								     e_fmt_kind[e_fmt_ix] = 'e';
								     e_fmt[e_fmt_ix++] = strdup(buf);
								     msd_simple |= MSD_NOCHD;
								   }

				when 'e':  
				case 'p':  if (e_fmt_ix >= NO_FMT)
								   { fputserr("Too many cmds\n");
								     myexit(0);
								   }
								   e_fmt_kind[e_fmt_ix] = ch;
								   e_fmt[e_fmt_ix++] = get_ee(true);
								   if (ch == 'e')
								     msd_simple |= MSD_NOCHD;
								   if (s[1] != ch && (s[1] == 'e' || s[1] == 'p'))
								   { e_fmt_kind[e_fmt_ix] = s[1];
								     e_fmt[e_fmt_ix] = strdup(e_fmt[e_fmt_ix-1]);
								     ++e_fmt_ix;
								     ++s;
//							     msd_simple |= MSD_NOCHD;
								   }

				when 'T':  msd_simple |= MSD_CHGD;

				when '\\': sep_ch = ch;

				when '/':{ Char * fpat = null;
								   Byte fname_props = conj | negprops;
								   Vint fprops = 0;

								   while ((ch = *++s) != 0)
								     switch (ch)
								     { when 'u': fname_props |= M_IC;
								       when 'r': fprops |= MSD_ROFILE;
								       when 'a': fprops |= MSD_CHGD;
								       when 'h': fprops |= MSD_HIDFILE;
								       when 's': fprops |= MSD_SYSFILE;
								       
								       when '/': fpat = s;
								       					 *s = '*';
																 s = ""; --s;
								     }

								   props[fname_props & 3] |= fprops;
								   --s;
								   if (fprops == 0)		/* pattern needed */
								   {
								     if (fpat == null)
								     { fpat = get_ee(false);
								       if (fpat == null)
												 explain();
								     }

								     if (fpat[strlen(fpat)-1] == '/')
								     { /* fpat[strlen(fpat)-1] = 0; */
								       fname_props |= M_DIRNM;
								     }

								     add_cell(&re_exp, fpat, (Byte)(fname_props | O_M_IC));
								     if (conj)
								       re_and += 1;
								     else
								       fn_or = true;
								     if (conj == 0 && negprops != 0 ||
												 conj != 0 && negprops == 0)
								       explain();
								   }
								 }
				when 'o':  ch = *++s - '0';
							     if (ch == '0')
							       ch = 1;
								   if (ch != 1 && ch != 2)
								     explain();
				when 's':
				case 'm':  msd_simple &= ~MSD_SIMPLE;
								 { Char * nxt = get_ee(false);
								   if (nxt == null)
								     explain();
								   if	   (ch == 's')
								   { Int sz = atol(nxt);
								     if (sz > 0)
								       s_lo_opt = sz;
								     else 
								       s_hi_opt = -sz;
								   }
								   else if (ch == 'm')
								   { Nat4 date = asc_to_tad(nxt);
						         if (grep_line(nxt, "^TODAY", M_MATCHWORD+M_IC) == OK ||
						             grep_line(nxt, "^NOW"  , M_MATCHWORD+M_IC) == OK)
						         { char ch = toupper(*nxt);
						           int offs = 0;
							         while (*++nxt != 0 && *nxt != '+' && *nxt != '-')
							           ;
							         offs = atoi(nxt) * 60;
#if S_MSDOS
						         { time_t now = time(null) + offs;
						           struct tm * res;
						         /*printf("Yr %d Mon %d Day %d\n", res->tm_year, res->tm_mon+1,res->tm_mday);*/
						           if (ch == 'T')
						           { now /= (24*60*60);
						             now *= (24*60*60);
                       }
                       res = gmtime(&now);
                       if (in_range(res->tm_mon, 5, 8) && ch == 'T')
                       { now -= 60 * 60;
                         res = gmtime(&now);
                       }
								       date = ((Int)(((res->tm_year-80)<< 9) +
	                             ((res->tm_mon+1) << 5) +
	                             res->tm_mday) << 16)
	                       +(res->tm_hour<< 11) +  /* was +? */
	                        (res->tm_min  << 5) +
	                        (res->tm_sec >> 1);
                     }
#else
                       date = time(null);
                       if (ch == 'T')
                       { date /= (24*60*60);
                         date *= (24*60*60);
                       }
                       date += offs;
#endif
                     }
                   /*printf("Date: %x\n", date);*/
								     if (date == (Nat4)-1)
								       explain();
								     if (s[1] != 'b')
								     { date_e_opt = date;
                     /*eprintf(null, "lopt %ld\n", date_e_opt);*/
                     }
								     else
								     { date_l_opt = date;
								       ++s;
								     }
								   { /* struct tm * td = gmtime(&date_e_opt);
								     eprintf(null, ">> %s <<\n", ctime(&date_e_opt));
								     eprintf(null, "day %d month %d DTS %d ONE %d\n", td->tm_mday, td->tm_mon, td->tm_isdst,1);
								     eprintf(null, "hh %d mm %d\n", td->tm_hour, td->tm_min);
								     td = localtime(&date_e_opt);
								     eprintf(null, "day %d month %d DTS %d ONE %d\n", td->tm_mday, td->tm_mon, td->tm_isdst,1);
								     eprintf(null, "hh %d mm %d\n", td->tm_hour, td->tm_min);
								     if (td->tm_isdst)
								     { eprintf(null, "is DST\n");
								       date_e_opt -= 3600;
								     }
											*/
								     /* eprintf(null, "Date Opt is %ld\n", date_e_opt);*/
								   }}
								   else
								   { unlink(nxt);
								     freopen(nxt, "a", ch == 1 ? stdout : stderr);
								   }
								 }
				when 'g':
				case 'G':{ Byte props = (ch == 'G') * M_IC |
																 conj | negprops   |				
																(s[1] != 0 ? 0 : M_MATCHWORD);
								   add_cell(&gr_pat, 
												    (props & M_MATCHWORD) ? get_ee(false) : &s[1],
												    props);
								   if (conj)
								     gr_and += 1;
								   s = "_";		/* any character would do!! */
								   msd_simple |= MSD_CHGD;
								 }
                                      /* 1) + from first non-blank */
        when 'x':                     /* 2) <sp>:anywhere, 4) X: ignore case */
        case 'X':{ Char * xstr = s[1] != 0 ? s+1 : get_ee(false);
                   exc_opt = (s[1] != 0 ? 2 : 0 ) | conj | 
                             (s[0] == 'X' ? 4 : 0);
                   if (strlen(xstr) < sizeof(exc_str))
                     strcpy(exc_str,xstr);
                   if (exc_opt < 0)
                     for (xstr = exc_str; *xstr != 0; ++xstr)
                       *xstr = toupper(*xstr);
                 }

				when 'd': nosl_opt = true;
		  						d_opt = true;
							  	msd_simple |= MSD_NOCHD;
					  			if (s[1] == 'y')
					  			{ d_opt = 2;
					   				++s;
					 				}
				case '+':  prepost_opts |= MSD_POST;
				when 'i':  i_opt = true;
				when 'l':  msd_simple &= ~MSD_SIMPLE;
								   long_opt = 1;
				when 'L':  msd_simple &= ~MSD_SIMPLE;
					  			 long_opt = 2;
				when 'u':  u_opt = true;
				when 'q':  q_opt = true;
				           if (s[1] == 'q') qq_opt = true;
				when 'V':  vv_opt = true;
				case 'v':  v_opt = 1;
				           if (s[1] == 'v') { qq_opt = true; ++s; }
				           if (in_range(s[1], '0','9'))
				           { v_opt = atoi(s+1);
				             while (in_range(s[1], '0','9'))
				               ++s;
				           }
				when 'S':  nosl_opt = true;
				when '-':  prepost_opts |= MSD_DIRY;
				when '#':  ct_opt = true;
				otherwise  if (! in_range(ch, '0','9'))
								   { eprintf(null, ">%s<\n", s);
								     explain();
								   }
								   rec_clamp = ch-'0';
			}
    }
  }}
  
  if ((msd_simple & (MSD_NOCHD+MSD_CHGD)) == (MSD_NOCHD+MSD_CHGD))
  { fputsout("-g(G) with -e not implemented\n\n");
    fflush(stdout);
    explain();
  }

  if (re_exp == null && *(Int*)&props[0] == 0 && d_opt)
  { props[IX_OR] |= MSD_ROFILE;
    props[IX_ORN] |= MSD_ROFILE;
  }
  g_filter = (Int)re_exp | s_lo_opt   | ((s_hi_opt+1) & 0x7fffffff) 
						| date_e_opt | (date_l_opt + 1);

  g_one_suff = 0;
  if (re_exp != null)
  { if (re_exp->next == null && (re_exp->props & M_NOT) == 0)
    { s = &re_exp->c[strlen(re_exp->c)-1];
      if (*s != '*')
      { g_one_suff = 1;
      //fputsout("One Suff\n");
      }
    }
  }
}

int main(
	int	argc,
	char ** argv)
{ fast Vint ix;
  invokenm = argv[0];
  if (argc <= 1)
    explain();

  stdip = stdin;
  tzset();

  if (strcmp(argv[argc-1], "-v") == 0)
		v_opt = 1;
  else if (strcmp(argv[argc-1], "-V") == 0)
		vv_opt = 1;
	else
		++argc;

  set_args(argc-1, argv, stdin);

  process_args();

  if (at_fn != NULL)
  { stdip = fopen(at_fn, "r");
    if (stdip == NULL)
    { printf("Cannot open file list file %s\n", at_fn);
      explain();
    }
  }

  for (ix = 0; ; ++ix)
  { Char * eny;
    Char * fn;
    Cc cc = OK;

    if (at_opt)
    { memset(&msd_stat, 0, sizeof(msd_stat));

      if (mygets0(&fnbuff[0], sizeof(fnbuff)-1) != OK)
				break;

      eny = fnbuff;
      fn = ".";

      if (eny[0] == 0)
        continue;
      if (at_opt == 1)
      {	cc = EDENIED;
      	fn = fnbuff;
      }
    }
    else
    { fn = get_ee(false);
      /*printf("NA %x %s\n", fn, fn);*/
      if (fn == null)
				if (ix == 0)
				  fn = ".";
				else
				  break;
      if (strcmp(fn, "..") == 0 && d_opt)
        continue;
      if (fn[0] == '-')
        explain();
        
    /*eprintf(null, "Filling %s\n", fn);*/
      cc = msd_init(fn, "",
           					MSD_DIRY | MSD_AUTO | MSD_POST |
									  (props[IX_OR] | props[IX_AND])&(MSD_HIDFILE+MSD_SYSFILE) |
										 msd_simple
							      );
    }
    
		if (cc != OK)
    { struct stat fstat_;
    	cc = stat(fn, &fstat_);
      if (cc != OK)
      { eprintf((char*)stderr, "File not found %s (%x)\n", fn, cc);
			  continue;
			} 
      msd_stat.st_size =  fstat_.st_size;
      msd_stat.st_mtime = fstat_.st_mtime;
#if S_MSDOS
      msd_attrs = msd_getprops(fn);
#else
      msd_attrs =((fstat_.st_mode & S_IFMT) == S_IFDIR ? MSD_DIRY   :
		  						(fstat_.st_mode & S_IREAD) != 0      ? MSD_ROFILE : 0) | 
		    /* crude!*/
						     (eny[0] == '.' ? MSD_HIDFILE : 0);
#endif
			if (!at_opt)
	    {	eprintf(null,"%s %d  ",fn, msd_stat.st_size);
      	eprintdet();
      	eprintf(null, "\n");
				continue;
			}
		}

  /*printf("Got file %s %s\n", fn, eny);*/

		if (a_opt && gr_first != NULL)
		{	fputsout(gr_first);
			fputsout("\n");
		}

  { int fnoffs=0;
    Vint dirl = ! q_opt || at_opt /*or ix>0 || argix_ != argc_-1*/
									? 0 : strlen(fn)+1;
    if (dirl > 0 && fn[dirl-2] == '/')
      --dirl;
    
    while (at_opt || (eny = msd_nfile(&fnoffs)) != null || pending_delete)
    { if (pending_delete)
      { pending_delete = false;
        /*eprintf(null, "DOPT PEND %s %x\n", cmd_line);*/
				erase_file(cmd_line, '/');
				if (eny == null)
				  break;
      }

      if (msd_attrs & MSD_POST)
				++rec_clamp;

    { Set16 is_diry = msd_attrs & (MSD_DIRY+MSD_POST);
    /*eprintf(null, "ATTS %s %d %x\n", eny, fnoffs, msd_attrs);*/
      if     (! is_diry &&
	      (props[IX_AND ] != (props[IX_AND ] & msd_attrs) ||
	       props[IX_ANDN] != (props[IX_ANDN] & ~ msd_attrs) ||
	      (!(props[IX_OR]+props[IX_ORN] == 0 || 
		  	(props[IX_OR] & msd_attrs) || 
			  (props[IX_ORN] & ~msd_attrs)))))
				;
      else
      { Set16 pset = want_file(&eny[fnoffs]);
        if (pset & M_DOPROC)
				  doit(pset, fn, eny, dirl);
        if ((msd_attrs & MSD_DIRY & pset) && 
				    (u_opt || !msd_isslink())  &&
	    			rec_clamp > 0 && (~msd_attrs & MSD_POST))
        { 
        /*eprintf(null, "MSDP(RPT) %s\n", eny);*/
				  if (msd_push() == EDENIED && vv_opt)
				  { char cwdb[160];
	    			eprintf((char*)stderr,"Cannot open %s (CWD %s)\n",eny,getcwd(cwdb,159));
				  }
				  --rec_clamp;
        }
      }

#if S_MSDOS && S_WIN32 == 0
      if (! i_opt && ! at_opt && typahead() && grabchar() == 'C'-'@')
      { fputserr("Interrupted");
	myexit(0);
      }
#endif
      if (at_opt)
				break;
    }}
  }}

#if S_MSDOS && 0
  if (file_ct > 0)
    fputsout("\r");

#endif
  if (ct_opt)
  { eprintf(null, "\n%d Files\n", file_ct);
    if (s_lo_opt >= 0)
      eprintf(null, "Total Size %ldk\n", file_tot_sz_k);
      
    if (cmp_dir != 0)
      eprintf(null, "%d Similar, %d Different\n", same_ct, diff_ct);
  }

/*
  if (! same)
    fputs("Not Same", stderr);
*/
  myexit(diff_ct & 0xff);
}
