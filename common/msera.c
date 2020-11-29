#include        <stdio.h>
#include        <ctype.h>
#include        <string.h>
#include        <fcntl.h>
#include        <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include        "build.h"
#if S_MSDOS
# include        <dos.h>
#else
# include        <stdlib.h>
#endif
#include 	"h/msera.h"


#if S_MSDOS && S_WIN32 == 0

union REGS rg;			   /* these structures are used throughout */
struct SREGS segreg;

static char temp[FILENAMESZ+6];


extern Cc ms_intdosx(Char * str, Short code);

Cc attrib_file(char * s, int attrib)

{ rg.x.cx = attrib;
  return ms_intdosx(s, 0x4301);
}


#if 0
					/* erase forward and back links */
static void erase_fbl()

{ Short ct = 2;
  while (--ct >= 0)
  { strcat(&temp[0], ".");
    erase_file(&temp[0], '^');
  }
}

#endif

#endif    
    
Cc erase_file(
	Char * s,
	Char   wh)		/* ' '=> file, '/' => dir, '^' => . or .. */
{ 
  Cc cc;
#if S_MSDOS && S_WIN32 == 0
  union REGS rg;
  struct SREGS segreg;
  int ct = 2;

  while (--ct >= 0)
  { cc = ms_intdosx(s, wh != ' ' ? 0x3a00 : 0x4100);
  						 /* delete file/directory */
    if (cc == OK)
      break;
    if (wh == '/' and strlen(temp) < FILENAMESZ-3)
     #if 0
     { strcpy(&temp[0], s);
       erase_fbl();
     } 
     #else
     { Char ch;
       strcpy(&temp[0], "rmdir ");

       for (cc = -1; (ch = s[++cc]) != 0; )
       { if (ch == '/')
	     ch = '\\';
	 temp[6+cc] = ch;
       }
       temp[5+cc] = 0;	  /* chop last '/' */
         
       shellprog(temp);
     }
     #endif
    if (wh == ' ')
      attrib_file(s,0);
  }

  return cc;
#else
  
  return wh != '/' ? unlink(s)
                   : rmdir(s);
#endif 
}
