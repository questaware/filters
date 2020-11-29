/*#include <stdio.h>*/
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "build.h"
#if S_MSDOS == 0
#include <unistd.h>
#endif
#include "strmpath.h"
#include "../common/h/msdir.h"

extern char * getcwd();


#if S_MSDOS
#define PATHSEPCH ';'
#else
#define PATHSEPCH ':'
#endif

static Char * sp_path = null;
static Char * sp_cand = null;
static Bool  sp_gotcwd = false;


static Char dbuff[FILENAMESZ+10];
static Char resbuff[FILENAMESZ+10];


Char * srchpath(
	Char    * cmdname,	/* name of the command including wildcards */
	Char *    path,		/* the path to search */
	Set	  opts)		/* see strmpath.h */
{ int trash;
  Bool haswild = false;

  Char * fn;
  Char * pth;

  for (fn = cmdname; *fn != 0; ++fn)
    if (*fn == '*' or *fn == '?')
    { haswild = true;
      break;
    }

  if (cmdname[0] == '.' and cmdname[1] == '/')
    cmdname += 2;

  if (sp_path == null)
    sp_path = path == null 			      ? ""   : 
              cmdname[0] == '/' or cmdname[0] == '\\' ? "::" : path;
  
  while (*sp_path != 0)
  { 
    Char * t = &dbuff[0];
  
    for (pth = sp_path; *pth != 0; ++pth)
    { if (*pth == PATHSEPCH)
      { ++pth;
        break;
      }
      *t++ = *pth;
    }
    
    if      (t <= dbuff)
    { if (sp_gotcwd)
      { sp_path = pth;
        continue;
      }
      sp_gotcwd = true;
    }
    else if (t[-1] != '/')
      *t++ = '/';
      
    if (t == dbuff)
    { *t++ = '.';
      *t++ = '/';
    }
    if (cmdname[0] == '/' or cmdname[0] == '\\')
      t = cmdname;
    else
      strcpy(&t[0], cmdname);
    
    if (sp_cand == null)
    { /*fprintf(stderr, "Trying %s\n", dbuff);*/
      msd_init(dbuff, null, MSD_DIRY | MSD_REPEAT | 
			    MSD_HIDFILE | MSD_SYSFILE);
    }
    sp_cand = msd_nfile(&trash);
  /*fprintf(stderr, "Got %s\n", sp_cand);*/

    if (sp_cand == null)
      sp_path = pth;
    else
    { struct stat xstat;
      Cc cc = stat(sp_cand, &xstat);
    
      if (cc == OK and 
          ((opts & M_EOPT) == 0 or (xstat.st_mode & 0001)
#if S_MSDOS == 0
      		        or (xstat.st_uid == getuid() and xstat.st_mode & 0100)
      			or (xstat.st_uid == getgid() and xstat.st_mode & 0010)
#endif
      	 ))
      { if (sp_cand[0] == '.' and sp_cand[1] == '/')
	  sp_cand += 2;

        resbuff[0] = 0;
        if (sp_cand[0] != '/' and
#if S_MSDOS
            sp_cand[1] != ':' and
#endif
                                  (opts & M_AOPT))
        {
          Char * cwd = (Char *)getcwd(&resbuff[0], FILENAMESZ+1);
          if (cwd == null)
            strcpy(&resbuff[0], "*?*?*");
          if (cwd != resbuff)		/* bit dodgy ? */
            strcpy(&resbuff[0], cwd);
          
          strcat(&resbuff[0], "/");
        }
        strcat(&resbuff[0], sp_cand);
        
        if (opts & M_DOPT)
        { 
          Char * sl = strrchr(resbuff, '/');
          if (sl != null)
            sl[0] = 0;
        }
        
        return resbuff;        
      }
    }
  }
  
  sp_path = null;
  sp_cand = null;
  sp_gotcwd = false;
  return null;
}



Char * abspath(Char * cmdname)

{ return srchpath(cmdname, getenv("PATH"), (long)M_AOPT);
}

