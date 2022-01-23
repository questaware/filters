/*!***************************************************
 *
 * SOURCE    : flook.c
 *
 * Unrestricted Code
 *
 * Ver  Date       Who   What
 * ~~~  ~~~~       ~~~   ~~~~
 * 0.1  15/09/1997 PJS   Adopted
 * 0.2  19/11/1997 PJS   Added function find_sys_root()
 *
 *****************************************************
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


#include "build.h"
#if S_MSDOS == 0
#include <stdlib.h>
#else
#include <io.h>
#endif

#include "strutil.h"

#if 1
extern char * getcwd();
#endif

#include        "build.h"


extern char * getenv();



#define ENVFUNC 1

#ifndef PATHCHR
#if S_MSDOS || S_BORLAND
#define PATHCHR ';'
#else
#define PATHCHR ':'
#endif
#endif


#ifndef NFILEN
#define NFILEN 255
#endif




Char * g_invokenm;

Char g_rootdiry[300];
                        /* allow prefixing: */
#define MID 150 


char homedir[30];


static char fspec[FILENAMESZ];	/* full path spec to search */


#if 0

/*!********************************************************
 char * nmlze_fname(tgt, src, tmp)
 
 Remarks
 ~~~~~~~
 Convert backslashes to forward slashes
 
 Return Value
 ~~~~~~~~~~~~
 The first parameter: tgt

 Parameters
 ~~~~~~~~~~
 char * tgt		the target
 char * src		the source string
 char * tmp             a temporary buffer
**********************************************************/

int nmlze_fname(tgt, src, tmp)
	char *   tgt;
	char *   src;
	char *   tmp;
{      char * cwd_ = getcwd(tmp, NFILEN);
       char * cwdend = &cwd_[strlen(cwd_)];
  fast char * t = tgt;
  fast char * s = src;
  fast char ch;
  fast int ct = NFILEN;
       int got_star = false;
       int depth = 0;
  do
  { ch = *s++;
    if      (ch == '\\')
      ch = '/';

    *t = ch;
    if (--ct >= 0)
      ++t;

    if      (ch == '*')
      got_star = true;
    else if (ch == '/')
    { Char * tt = t;
      *t = 0;
      t -= 4;
      if (t >= tgt)
      { if (strcmp(t+1, "/./") == 0)		/* /./ -> / */
        { t += 2;
          continue;
        }
        if (strcmp(t, "/../") == 0)		/* take out dir/.. */
        { for (; --t >= tgt and *t != '/'; )
            ;
          t += 1;
          if (t[0] != '.')
            continue;
        }
      }
      t = tt;
    }
  }
  while (ch != 0);
  
  t = tgt;
  
{ Char * cw = cwdend;

  while (*strmatch("../", t) == 0 and cw >= cwd_)
  { t += 3;						  /* target forward */
    while (--cw >= cwd_ and *cw != '/' and *cw != '\\') /* cwd backward */
      ;
  }
  if (cw != cwdend)
  { Char * ncw = strmatch(cw+1, t);
    if ((*ncw == 0 or *ncw == '\\' or *ncw == '/') and t[ncw - cw - 1] == '/')
    { s = t;
      for ( ; ++cw <= ncw and t >= tgt+3; )
      { ++s;
        if (*cw == 0 or *cw == '\\' or *cw == '/')
          t -= 3;
      }
      strcpy(t, s);
    }
  }

  return got_star;
}}

#endif

/*!********************************************************
 Bool fexist(char * fname)
 
 Remarks
 ~~~~~~~
 Indicates whether or not the file exists and is readable
 
 Return Value
 ~~~~~~~~~~~~
 As above

 Parameters
 ~~~~~~~~~~
 char * fname		the name of the file from the current directory
			or absolute
**********************************************************/

Bool fexist(char *fname)	/* does <fname> exist on disk? */
				/* file to check for existance */
{
#if 0
  char tfn[FILENAMESZ+2];
  fname = nmlze_fname(&tfn[0], fname);
#endif

{ Cc cc = access(fname, 0);
    /* fprintf(stderr, "Does %s exist? %d\n", fname, cc);*/
  return cc == OK;
}}

/*!********************************************************
 char * flook(char wh, char * fname)
 
 Remarks
 ~~~~~~~
	wh == I => 
	Look up the existence of a file in the directory of curbp or 
	along the include path in the INCLDIRS environment variable. 
	else 
	  if wh is in uppercase then look first in the $HOME directory
	  always look up a file along the normal $PATH.
 
 Return Value
 ~~~~~~~~~~~~
 null => the file cannot be found
 else	 the absolute location of the file

 Parameters
 ~~~~~~~~~~
 char * fname		the name of the file

**********************************************************/

char * flook(
	char  wh,
	char *fname)	/* base file name to search for */
{
           char uwh = toupper(wh);
  register char *sp = fname;
#define u_sp ((unsigned char*)sp)
  Int front = (((u_sp[2] << 8) | u_sp[1]) << 8) | u_sp[0];
  if (u_sp[0] == 0)
    return null;
			/* if we have an absolute path.. check only there! */
  if (*sp == '\\' || *sp == '/' || sp[1] == ':')
    return not fexist(sp) ? null : sp;
					/* try the current directory first */
{ Char * cwd = sp[0] != '.' ? null : getcwd(&fspec[0], sizeof(fspec));
  if (cwd != null)
  { Vint cwdix = strlen(fspec);

    for (; (front & 0xffff) ==       ('.'+'/' * 256) or 
	    front	    == '.' + '.' * 256 + '/' * 256 * 256
	 ; front = (((u_sp[2] << 8) | u_sp[1]) << 8) | u_sp[0])
    { if (sp[1] != '.')			  /* case ./ */
	sp += 2;
      else				  /* case ../ */
      { sp += 3;
        while (--cwdix >= 0 and cwd[cwdix] != '/')
	  ;
      } 
      while (--cwdix > 0 and cwd[cwdix] == '/')
	;
      if (cwdix < 0)
      { sp = fname;			/* doesnt match!! */
	break;
      }
    }
  
    if (sp != fname)			/* we did something */
    { cwd[++cwdix] = '/';
      strpcpy(&cwd[cwdix+1], sp, sizeof(fspec)-cwdix-1);
      return fspec;			/* The (c)wd || the filename */
    }
  }
					/* then try the program directory */      
  if (g_invokenm != null)
  { strcpy(&fspec[0], g_invokenm);
    for (sp = &fspec[strlen(fspec)]; --sp >= &fspec[0]; )
      if (*sp == '/' or *sp == '\\')
      { strcpy(&sp[1], fname);
        if (fexist(fspec))
          return fspec;
        break;
      }
  }
					/* then try $HOME */
#if     ENVFUNC
  if (wh < 'a')
  { if (homedir[0] == 0)
#if S_MSDOS
      strcpy(&homedir[0], "c:/");
#else
    { Char * hm = getenv("HOME");
      if (hm != null)
        strncpy(&homedir[0], hm, sizeof(homedir)-2);
    }
#endif
    if (homedir[0] != 0)		      /* build home dir file spec */
    { strcat(strcat(strcpy(&fspec[0], homedir), "/"), fname);
      if (fexist(fspec))
        return fspec;
    }
  }
					/* then try the {INCLUDE} PATH */
{ register char *path;	/* environmental PATH variable */
  fspec[0] = 0;
  if	  (uwh == 'I')
    path = getenv("INCLDIRS");
  else if (*fname != '\\' && *fname != '/')
  { path = getenv("PATH");
  /*eprintf(null, "PATH: %s\n", path);*/
  }
  else
  { strcpy(&fspec[0], fname);	  /* what the hell is this? */
  { 
    char * t = strrchr(fspec, *fname);
    if (t != null)
      *t = 0;
    path = &fspec[0];
  }}

  if (path != null)
    for (; *path != 0; ++path)
    { if (path[0] == '-' and path[1] == 'I')
	path += 2;
    { char * start = path;
      sp = fspec;		       /* build next possible file spec */
      
      for (; *path != 0 && *path != ' ' && *path != PATHCHR
	   ; ++path)
	*sp++ = *path;
      
      if (sp == fspec)                /* :: counts as :.: */
        *sp++ = '.';
      
      if (path[-1] != DIRSEPCHAR)
	*sp++ = '/';
	  
      strcpy(&sp[0], fname);
   /* eprintf(null, "FEX: %s\n", fspec); */
	  
      if (fexist(fspec))
	return fspec;
    }}
}
#endif
#if 0
  if (uwh == 'I')
  { strcat(strcpy(&fspec[0], "/usr/include/"), fname);
    return fexist(fspec) ? fspec : null;
  }
#endif
  return null;	/* no such luck */
}}




/*!********************************************************
 Char * find_sys_root(Char * name, Char * suffix, Char envvar)
 
 Remarks
 ~~~~~~~
  Find the directory containg the command, then strip off
  the suffix, storing the result in g_rootdir[], and in 
  the environment variable envvar if not already set.
 
 Return Value
 ~~~~~~~~~~~~
 null => Internal Error caused root not to be found.
 else	 the absolute location of the system containg the
 	 file for this binary.

 Parameters
 ~~~~~~~~~~
 Char * name		the name of the binary relative to $PATH
 Char * suffix		the name of the directory of the binary 
 			from the root of the system directory tree.
 Char * envvar		An environment variable giving the root.
**********************************************************/
Char * find_sys_root(Char * name_, Char * suffix, Char * envvar)

{ 
#if 0
	Char nml_name[1024];
  Char work_space[1024];
  int rc = nmlze_fname(&nml_name[0], name_, &work_space[0]);
  Char * name =        &nml_name[0];
#else
 #define name name_
#endif
  Bool strip = false;
  Char * rd_ = getenv(envvar);
  Char * rd = rd_;
  if (rd != null)
    strip = rd[strlen(rd)-1] == '/';
  else						/* use flook() */
  { rd = flook('P', name);
    /*fprintf(stderr, "flook %s\n", rd);*/
    strip = true;
  }
  if (rd != null)
  {
    g_rootdiry[0] = 0;                          /* start by using MID */
    strpcpy(&g_rootdiry[MID], rd, sizeof(g_rootdiry)-MID);
    if (strip)
    { Vint ix;					/* strip down to the directory */
      for (ix = MID+strlen(rd); --ix > MID and g_rootdiry[ix] != '/'; )
        ;
        
      g_rootdiry[ix] = 0;
      if ( g_rootdiry[MID] == '.' and 
          (g_rootdiry[MID+1] == 0 or g_rootdiry[MID+1] == '/'))
      { (void)getcwd(&g_rootdiry[0], MID-1);
        strpcat(&g_rootdiry[0], &g_rootdiry[MID+1], sizeof(g_rootdiry));
      }
    }
    if (g_rootdiry[0] == 0)                     /* rebase from MID to 0 */
      strpcpy(&g_rootdiry[0], &g_rootdiry[MID], MID);

  { int rix, wix = -1;                               /* strip out multiple slashes */
    for (rix = -1; g_rootdiry[++rix] != 0;)
      if (g_rootdiry[rix]   == '/' and 
          g_rootdiry[rix+1] == '/')
        ;
      else
        g_rootdiry[++wix] = g_rootdiry[rix];

    g_rootdiry[++wix] = 0;

    if (rd_ == null)				/* not got environment variable */
#if 1
    { Vint six = strlen(suffix);
      Vint ix = strlen(g_rootdiry);
      if (ix <= six)
        return null;
                                              /* strip off the suffix */
      while (ix > 2 and
             g_rootdiry[ix-1] == '.' and
             g_rootdiry[ix-2] == '/')
        ix -= 2;
        
      while (--six >= 0 and suffix[six] == g_rootdiry[--ix])
        ;
      
      if (six >= 0 or ix < 0)
      { /*fprintf(stderr, "%d %d %s", six, ix, g_rootdiry);*/
        return null;
      }
      g_rootdiry[ix] = 0;
      
    { Vint used = strlen(g_rootdiry);
      Char * tgt = &g_rootdiry[used+1];	/* use space at end of array */
      if (used + strlen(envvar) >= sizeof(g_rootdiry)-2)
        exit(27);	/* keep it simple, dont use log.c or malloc */
      sprintf(&tgt[0], "%s=%s", envvar, g_rootdiry);
      fprintf(stderr, "Putenv %s\n", tgt);
      putenv(tgt);
    }}
#endif
    strpcpy(&g_rootdiry[MID], rd, sizeof(g_rootdiry)-MID);
    rd = &g_rootdiry[0];
  }}
  
  return rd;
}

