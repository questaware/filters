#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "build.h"

#include "../common/h/eprintf.h"
#include "../util/h/strutil.h"


extern Int atol();



#define MAXLN 1024

static char ipline[MAXLN+2];
static char opfname[MAXLN+2];


Bool e_opt = false;
Bool i_opt = false;
Bool j_opt = false;			/* Allow column names to follow space */
Bool z_opt = false;			/* z_opt => suppress blank lines */
Bool under_opt = false;

Vint sep_opt = 1;
Vint e_sep = 1;

#define MAX_COLS 100
#define MAX_COL_W 100


typedef struct
{ Char col_head[MAX_COL_W];
  Vint stt_offs;
} Column_t;

Column_t g_cols[MAX_COLS];

Vint g_col_wids[MAX_COLS];
Vint g_col_maxs[MAX_COLS];

Char g_sepch = ' ';			/* separator character */

Vint g_nix_col = 0;			/* Next column index */
Vint g_next_col = 0;			/* place on paper with free space */



#define MAX_COL_DPTH 7


char g_hd_rows[MAX_COL_DPTH] [MAXLN+2]; /* row being assembled */
char g_op_row [MAXLN+2];		/* row being assembled */

Bool g_hd_is_new = false;		/* the header has changed */


Bool g_got_data = false;		/* read a non null line */
Bool g_ignore_names = false;		/* ignore the field names */



private void explain()

{ fprintf(stderr, 
  "tocol {-, sep} {-# #} {-e{#} } {-i} {-z} {-s #} { filename }*\n"
  "   Put attributes in columns\n"
  "   -, sep   Separator character\n"
  "   -# #     Column # has width #\n"
  "   -e{#}    only evaluate column widths, allow sep of # spaces DEFAULT 1\n"
  "   -i       ignore subsequent column names\n"
  "   -j       Allow column names to follow space\n"
  "   -s #     # chars separate columns\n"
  "   -z       do not output blank lines\n"
  "   -_       put --- after header\n"
         );
  exit(0);
}



void myabort(Char * msg)

{ fprintf(stderr, msg);
  exit(1);
}



Bool is_diry(fd)

{	struct stat fstat_;
	int cc = fstat(fd, &fstat_);
	return cc == OK and (fstat_.st_mode & 040000) != 0;
}


Char * skipdigs(Char * src)

{ while (in_range(*src, '0', '9'))
    ++src;
  return src;
}



Char * skiptochar(Char * src, Char ch)

{ while (*src != ch and *src != 0)
    ++src;
  return src;
}




void insert_string(Char * tgt, Vint tgt_offs, Char * src, Vint len)

{ Char * t = &tgt[tgt_offs];
 
  if (*t == 0)
  { while (--tgt_offs >= 0 and tgt[tgt_offs] == 0)
     tgt[tgt_offs] = ' ';
  }

  len -= tgt_offs;
  memset(&t[0], 0, len - 1);

  while (*src != 0 and --len >= 0)
    *t++ = *src++;
}




Vint add_column(/*out*/Bool * is_new_ref,/*out*/Vint * offs_ref, Char * str)

{ Vint ix;
  Vint offs;
  *offs_ref = 0;
  *is_new_ref = false;
  
  if (g_ignore_names)
  { *offs_ref = g_cols[ix].stt_offs;
    return g_nix_col;
  }
  
  for (ix = -1; ++ix <= upper_index(g_cols) and g_cols[ix].col_head[0] != 0; )
  { if (strcmp(   g_cols[ix].col_head, str) == 0)
    { *offs_ref = g_cols[ix].stt_offs;
      return ix;
    }
  }
  
  if (ix > upper_index(g_cols))
    return -1;
  
  strpcpy(&g_cols[ix].col_head[0], str, sizeof(g_cols[ix].col_head));

  *offs_ref = 
  g_cols[ix].stt_offs = g_next_col;
  g_next_col = sep_opt + g_cols[ix].stt_offs + 
	   (g_col_wids[ix] != 0 ? g_col_wids[ix] : strlen(str));

  *is_new_ref = true;
  return ix;
}


Vint find_header_row(Vint offs)

{ Vint ix;
  if (offs == 0)
    offs = 1;

  for (ix = -1; ++ix < upper_index(g_hd_rows); )
    if (strlen(g_hd_rows[ix]) < offs)
      return ix;
  return -1;
}

void main(argc, argv)
	short	argc;
	Char ** argv;
{ Char * inpr;
  Int tot = 0;
  
  Bool d_opt = false;
  Bool m_opt = false;
  Bool got_all = false;
  Bool got_one_file = false;
  Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];
  
  for (; ; --argsleft)
  { if (argsleft > 0 and argv_[0][0] == '-' and argv_[0][1] != 0)
    { if      (argv_[0][1] == ',')
      { g_sepch = argv_[0][2];
	if (g_sepch == 0)
	{ --argsleft;
	  ++argv_;
	  g_sepch = argv_[0][0];
	  if (g_sepch == 0)
	    explain();		
	}
      }
      else if (argv_[0][1] == '_')
	under_opt = true;
      else if (argv_[0][1] == 'e')
      { e_opt = true;
	if (argv_[0][2] != 0)
	  e_sep = atoi(argv_[0][2]);
      }
      else if (argv_[0][1] == 'i')
	i_opt = true;
      else if (argv_[0][1] == 'j')
	j_opt = true;
      else if (argv_[0][1] == 'z')
	z_opt = true;
      else if (argv_[0][1] == 's')
      { sep_opt = atoi(&argv[1][0]);
	--argsleft;
	++argv_;
      }
      else if (in_range(argv_[0][1], '1', '9'))
      { Vint ix = atoi(&argv_[0][1]);
	--argsleft;
	++argv_;
	if (ix <= MAX_COLS)
	  g_col_wids[ix-1] = atoi(&argv_[0][0]);
      }
      else
      { explain();
      }
    }
    else
    { 
      Char * fn = argsleft <= 0 ? "-" : argv_[0];
      FILE * chan = fn[0] == '-' and fn[1] == 0 ? stdin : fopen(fn, "r");

      Char tfn[30];
#define tfile &tfn[3]
      sprintf(&tfn[0], "rm /tmp/pjstc%d", getpid());
    { FILE * op = fopen(tfile, "w");
      
      if (chan == null)
      { fprintf(stderr, "Cannot open file %s\n", fn);
	continue;
      }
      if (op == null)
      { fprintf(stderr, "Cannot open output file %s\n", tfile);
	continue;
      }
      
      memset(&g_cols[0], 0, sizeof(g_cols));
      
    { 
      while (true)
      { Char * ln = fgets(&ipline[0], MAXLN, chan);
	if (ln == null)
	  break;
	ln[strlen(ln)-1] = 0;
	if (j_opt)
	  ln = skipspaces(ln);
	if (ln[0] == 0 or not isalpha(ln[0]))
	{ if (not z_opt)
	    fprintf(op, "\n");
	  g_nix_col = 0;
	  if (i_opt)
	    g_ignore_names = true;
	  continue;
	}

	g_got_data = true;
      
      { Char * val = skipspaces(skiptochar(ln, g_sepch));
	*skiptochar(ln, g_sepch) = 0;
      
      { Bool is_new;
	Vint offs;
	Vint ix = add_column(&is_new, &offs, ln);
	if (e_opt)
	{ Int wid = strlen(val)+1;
	  if (g_col_maxs[ix] < wid)
	    g_col_maxs[ix] = wid;
	  continue;
	}
	if (is_new)
	{ Vint cix = find_header_row(offs);
	  if (cix >= 0)
	  { Vint len = strlen(g_hd_rows[cix]);
	    if (len < offs)
              memset(&g_hd_rows[cix][len], ' ', offs-len);
	    strpcat(&g_hd_rows[cix][0], "                                ", MAXLN);
	    strpcpy(&g_hd_rows[cix][offs], ln, MAXLN - offs);
	    g_hd_is_new = true;
	  }
	}
	
	++g_nix_col;
	
	if (offs == 0)
	{ if (g_op_row[0] != 0 or not z_opt)
	    fprintf(op, "%s\n", g_op_row);
	  memset(&g_op_row[0], 0, sizeof(g_op_row));
	  g_nix_col = 0;
	}
	insert_string(g_op_row, offs, val, sizeof(g_op_row));
      }}}
	
      fprintf(op, "%s\n", g_op_row);
      
      fclose(op);
      
    { Vint ix;
      if (not e_opt)
	for (ix = -1; ++ix < MAX_COL_DPTH and g_hd_rows[ix][0] != 0; )
	  printf("%s\n",  g_hd_rows[ix]);
      
      if (under_opt and not e_opt)
      { for (ix = g_next_col; --ix >= 0; )
	  printf("-");
	printf("\n");
      }

    { FILE * ip = fopen(tfile, "r");
      if (ip == null)
      { printf("Internal Error\n");
	continue;
      }
      
      while (true)
      { Char * ln = fgets(&ipline[0], MAXLN, ip);
	if (ln == null)
	  break;
	fputs(ln, stdout);
      }
      
      fclose(ip);
      fclose(chan);
      system(tfn);
      if (argsleft <= 1)
	break;
    }}}}}

    ++argv_;
  }
  
  if (e_opt)
  { Vint ix;
    for (ix = -1; ++ix < MAX_COLS; )
      if (g_col_maxs[ix] != 0)
	printf("-%d %d ", ix+1, g_col_maxs[ix]+e_sep-2);
    printf("\n");
  }

  exit(0);
}

