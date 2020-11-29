#pragma warning(disable : 4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <io.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "build.h"




static void explain(void)

{ fputs("striphtm {-t} {-T} filename*\n"
        "  -t  table contents only\n"
        "  -T  DELIMIT TABLES TABLE..ETABLE\n"
        "Strip htm commands\n", stderr
         );
  exit(0);
}




static int t_opt = 0;
static int T_opt = 0;

static int ip_file;

static char tag[100];
static int  imgix;		/* place where image name starts */
static int  imgend;		/* place where image name ends */

Bool in_cmt = false;



char get_sp_char(void)

{ char chb[6];
  int ix = -1;

  while (++ix < 4)
  { int cc = read(ip_file, &chb[ix], 1);
    if (cc <= 0)
      break;
      
    if (chb[ix] != 'n' && !in_range(chb[ix], 'a', 'z') &&
                          !in_range(chb[ix], '0', '9') &&
        chb[ix] != '#'
       )
      break;
  }
  chb[ix] = 0;
  
  return strcmp(chb, "nbsp") == 0 ? ' ' : 
         strcmp(chb, "quot") == 0 ? '\'':
         strcmp(chb, "amp") == 0  ? '&' :
         strcmp(chb, "#163") == 0 ? 'œ' : '?';
}




char * get_tag(void)

{ int ix = -1;
  Bool cmt = false;
  Bool img = false;
  int minct = 0;
  imgix = -1;

  while (1)
  { int cc = read(ip_file, (void *)&tag[++ix], (unsigned)1);
    if (cc <= 0)
      break;
      
    if (tag[ix] == '>')
    { 
      if (minct <= 0)
        in_cmt = false;
      
      break;
    }

    if (tag[ix] != '-')
      minct = 2;
    else
      minct -= 1;
      
    if (ix == 2 && tag[0] == '!'
                && tag[1] == '-' 
                && tag[2] == '-')
    { minct = 2;
      in_cmt = true;
    }
    if (ix == 2 && tag[0] == 'i'
                && tag[1] == 'm' 
                && tag[2] == 'g')
      img = true;
    if (img and tag[ix]   == '"' and
                tag[ix-1] == '=' and
                tag[ix-2] == 'c' and
                tag[ix-3] == 'r' and
                tag[ix-4] == 's')
      imgix = ix;

    if (ix >= sizeof(tag) - 2)
      --ix;
  }
  
  tag[++ix] = 0;
  tag[++ix] = 0;
  if (imgix > 0)
  { register char ch;
    for (imgend = imgix; (ch = tag[++imgend]) != 0 and ch != '"'; )
      ;
    tag[imgend] = 0;
  }

  return tag;
}




main(argc, argv)
    int argc;
    char ** argv;
{ if (argc <= 1)
    explain();
    
{ Char buff[10];
  Char ** argv_ = &argv[1];
  int argsleft;

  for (argsleft = argc; --argsleft > 0; )
  { if (argv_[0][0] == '-' && argv_[0][1] != 0)
    { Char * s = &argv_[0][0];
      while (*++s != 0)
      { if      (*s == 'T')
	{ /*++argv_;
	  speed = atol(argv_[0]);*/
	  T_opt = 1;
	}
        else if (*s == 't')
	  t_opt = 1;
	else 
	  explain();
      }
    }
    else
    { Bool inhib = t_opt;
      int spct = 0;
      int level = 0;
      Bool pre = false;
      Bool in_script = false;
      int trix;
      int tdix;
      char * fn = argv_[0];
      
      ip_file = fn[0] == '-' ? 0 : open(fn, O_RDONLY);
      if (ip_file < 0)
      { 
        fputs("Cannot open ", stderr);
        fputs(fn, stderr);
        fputs("\n", stderr);
      }
      else
	while (1)
	{ int cc = read(ip_file, &buff[0], (unsigned int)1);
	  if (cc <= 0)
	    break;

	  if      (buff[0] == '<')
	  { char * cmd = get_tag();
	    /*putchar('(');*/

	    if (1 || t_opt or T_opt)
	    { if      (strnicmp(cmd, &"/table"[1], 5) == 0 and !isalpha(cmd[5]))
	      { level = 1;
	        trix = 0;
	        if (T_opt)
	          puts("TABLE");
	      }
	      else if (strnicmp(cmd, "/table", 6) == 0 and !isalpha(cmd[6]))
	      { level = 0;
	        if (T_opt)
	          puts("\nETABLE");
	      }
	      else if (strnicmp(cmd, &"/tr"[1], 2) == 0 and !isalpha(cmd[2]))
	      { if (trix > 0)
	          puts("");
	        level = 2;
	        ++trix;
	        tdix = 0;
	      }
	      else if (strnicmp(cmd, "/tr", 3) == 0 and !isalpha(cmd[3]))
	        level = 1;
	      else if (strnicmp(cmd, &"/td"[1], 2) == 0 and !isalpha(cmd[2]))
	      { if (tdix != 0 && (t_opt || T_opt))
	          printf(",");
	        level = 3;
	        ++tdix;
	      }
	      else if (strnicmp(cmd, "/td", 3) == 0 and !isalpha(cmd[3]))
	        level = 2;
	      else if (strnicmp(cmd, &"/pre"[1], 3) == 0 and !isalpha(cmd[3]))
	        pre = true;
	      else if (strnicmp(cmd, "/pre", 4) == 0 and !isalpha(cmd[4]))
	        pre = false;
	      else if (imgix > 0)
	      { fputs("IMG(", stdout);
        	fputs(&cmd[imgix], stdout);
	        fputs(") ", stdout);
	      }
	    }
	    if (strnicmp(cmd, &"/script"[1], 6) == 0 and !isalpha(cmd[6]))
	      in_script = true;
	    else if (strnicmp(cmd, "/script", 7) == 0 and !isalpha(cmd[7]))
	      in_script = false;
	    else if (strnicmp(cmd, "br", 2) == 0 and !isalpha(cmd[2]) &&
	    	     (t_opt + T_opt == 0 || level < 2)
	            )
	      puts("");
	    
	    /*putchar(')');*/
	  }
	  else if (in_script)
	    ;
	  else if (buff[0] == '&')
	  { //putchar('{');
	    putchar(get_sp_char());
	    //putchar('}');
	  }
	  else if (level > 0 && buff[0] == '\n')
	    /*printf("SuPp")*/;
	  else if (level > 0 && buff[0] == '\r')
	    /*printf("SupP")*/;
	/*else if (level > 0 && buff[0] == 13)
	    ;*/
	  else if (inhib && level < 3)
	    ;
	  else
	  { if (!pre)
	      if (isspace(buff[0]))
	        spct += 1;
	      else
	        spct = 0;
	  /*if (buff[0] < ' ' or buff[0] < '0' or buff[0] > 'z')
	      printf("CHR(%x)", buff[0]);
	    else*/
	    /*printf(in_cmt?"`%x":",%x", buff[0]);*/
	    if (spct < 2 && !in_cmt)
 	      putchar(buff[0]);
 	  }
	}
    }

    ++argv_;
  }

  return 0;
}} 

