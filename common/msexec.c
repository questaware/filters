#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <process.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "build.h"
#include "h/msera.h"

extern Char * flook();


#if S_MSDOS == 0
  MSDOS only
#endif

#if S_WIN32 == 0
union REGS rg;
struct SREGS segreg;


#if MSC
 extern int _doserrno;
#endif
#endif

/*
void fprintf()
{ puts("BLAHBLAH\n");
}

void sprintf()
{ fprintf();
}

void printf()
{ fprintf();
}
*/

/*	EXECPROG:	A function to execute a named program
			with arguments
*/

#if	LATTICE | AZTEC | MWC
#define	CFLAG	1
#endif

Cc mse_cc;


#if S_MSDOS && S_WIN32 == 0

short execprog(cmd)
	char *cmd;	/*  Incoming command line to execute  */
{
    char f1[38];	    /* FCB1 area (not initialized */
    char f2[38];	    /* FCB2 area (not initialized */
    char prog[NSTRING+10];  /* program filespec */

		    /* parse the command name from the command line */
    static const char exetyps[3][5] = { ".com", ".exe", ""};
    register char * sp;
    register short ix = 0;
    register char ch;

    for (; (ch = *cmd) != 0 && ch != ' ' && ch != '\t'; ++cmd)
      prog[ix++] = ch;

	     /* look up the program on the path trying various extensions */
    for (ch = 3; ; )
    { if (--ch < 0)
	return false;
      strcpy(&prog[ix], &exetyps[ch][0]);
      sp = flook('P', prog);
      if (sp != null)
	break;
    }
    strcpy(prog, sp);

#define progl ix
#define tail sp
    progl = strlen(prog);
    tail = &prog[progl+1];
				       /* and parse out the command tail */
    while ((ch = *cmd) == ' ' || ch == '\t')
      ++cmd;
    
    tail[0] = (char)strlen(cmd);   /* record the byte length */
    if (progl + tail[0] > NSTRING + 8)
      tail[0] = NSTRING + 8 - progl;

    strpcpy(&tail[1], cmd, tail[0]+1);
    strcat(&tail[0], "\r");
    
#if  MWC == 0
		/* get a pointer to this PSPs environment segment number */
    segread(&segreg);
#endif
{ struct Pblock 		/* set up EXEC parameter block */
  {   short envptr;   /* 2 byte pointer to environment string */
      char *cline;    /* 4 byte pointer to command line */
      char *fcb1;     /* 4 byte pointer to FCB at PSP+5Ch */
      char *fcb2;     /* 4 byte pointer to FCB at PSP+6Ch */
  } pblock;
    pblock.envptr = 0;    /* make the child inherit the parents env */
    pblock.fcb1 = f1;             /* point to a blank FCB */
    pblock.fcb2 = f2;             /* point to a blank FCB */
    pblock.cline = tail;          /* parameter line pointer */

			    /* and make the call */
    rg.x.ax = 0x4b00;	    /* EXEC Load or Execute a Program */
			    /* load end execute function subcode */
    rg.x.dx = (unsigned int)prog;
    rg.x.bx = (unsigned int)&pblock;
#if	AZTEC | MWC
    rg.x.ds = ((unsigned long)prog) >> 16;    /* program name ptr */
    rg.x.es = rg.x.ds;
  /*rg.x.es = ((unsigned long)&pblock) >> 16;  * set up param block ptr */
#else
    segreg.ds = ((unsigned long)prog) >> 16;	/* program name ptr */
    segreg.es = ((unsigned long)&pblock) >> 16; /* set up param block ptr */
#endif

#if	NOVELL
    mse_cc = execpr(prog, &pblock);
#else
#if	LATTICE 
    if (intdosx(&rg, &rg, &segreg) & CFLAG)
      mse_cc = -_oserr; 	    /* failed child call */
    else
    { rg.h.ah = 0x4d;	    /* get child process return code */
      intdos(&rg, &rg);     /* go do it */
      mse_cc = rg.x.ax;     /* save child's return code */
    } 
#endif
#if	AZTEC
    if (sysint(0x21, &rg, &rg) & CFLAG)
      mse_cc = -errno;		    /* failed child call */
    else
    { rg.h.ah = 0x4d;	    /* get child process return code */
      sysint(0x21, &rg, &rg);	    /* go do it */
      mse_cc = rg.x.ax;     /* save child's return code */
    } 
#endif
#if	MWC
    intcall(&rg, &rg, DOSINT);
    if (rg.x.flags & CFLAG)
      mse_cc = -errno;		    /* failed child call */
    else
    { rg.h.ah = 0x4d;	    /* get child process return code */
      intcall(&rg, &rg, DOSINT);    /* go do it */
      mse_cc = rg.x.ax;     /* save child's return code */
    } 
#endif
#if	(TURBO | MSC | DTL)
    intdosx(&rg, &rg, &segreg);
    if (rg.x.cflag != 0)
      mse_cc = -_doserrno;  /* failed child call */
    else
    { rg.h.ah = 0x4d;	    /* get child process return code */
      intdos(&rg, &rg);     /* go do it */
      mse_cc = rg.x.ax;     /* save child's return code */
    } 
#endif
#endif
    return mse_cc >= 0;
}}




/*	SHELLPROG: Execute a command in a subshell		*/

short shellprog(cmd)
	 char *cmd;	/*  Incoming command line to execute  */
{	char comline[NSTRING];	/* constructed command line */

		/*  detect current switch character and set us up to use it */
	rg.x.ax = 0x3700;	/*  get setting data  */
				/*  al: get switch character to use */
	intdos(&rg, &rg);
{	char swchar = (char)rg.h.dl;
	extern char * getenv();
					/*  get name of system shell  */
	char * shell = getenv("COMSPEC");
	if (shell == null)
	  return false;			/*  No shell located  */
				
	while (*cmd == ' ' || *cmd == '\t')
	  cmd++;
		 /**  If the command line is not empty, bring up the shell  **/
		 /**  and execute the command.  Otherwise, bring up the     **/
		 /**  shell in interactive mode.   **/
	if (*cmd)
	{ strcat(strcat(strcpy(comline, shell), " ?c "), cmd);
	  comline[strlen(shell) + 1] = swchar;		/* replace the ? */
	  shell = comline;
	}
	return execprog(shell);
}}

#else

Short execprog(cmd)
        Char *cmd;
{
  char	*args[12];
	/* Parse cmd into an argv.  Handle various quotes
	 * but not environment variable references.
	 */
  char *ip = cmd,
       *op = cmd,
      **ap = args;

  for (;;) 
  { 
    while (isspace(*ip))
      ip++;
    if (*ip == '\0')
      break;
    if (ap == &args[sizeof(args)/sizeof(args[0])])
    { mse_cc = ENOSPACE;
      return false;
    }
    *ap++ = op;
    for (;;) 
    { char c2, c = *ip++;

      switch (c)
      { case '\0':
    	    ip -= 1;
    	    /*FALLTHROUGH*/
        case ' ':
        case '\t':
    	    break;
        case '"':
        case '\'':
    	    while ((c2 = *ip++) != c)
	    { switch (c2)
	      { case '\0':
		      mse_cc = ESYNTAX;
		      return false;
		      /*NOTREACHED*/
	        case '\\':
		      if (c == '"')
		      { c2 = *ip++;
		        if (c2 == '\0')
		        { mse_cc = ESYNTAX;
		          return false;
		        }
		      }
		      /*FALLTHROUGH*/
	        default:
		      *op++ = c2;
	      }
	    }
            continue;
#if S_WIN32 == 0
        case '\\':
    	    c = *ip++;
    	    if (c == '\0')
	    { mse_cc = ESYNTAX;
	      return false;
	    }
#endif
    	    /*FALLTHROUGH*/
        default:
    	    *op++ = c;
    	    continue;
      }
      break;
    }
    *op++ = '\0';
  }
  *ap = null;

	/* Exec the parsed command */
#if S_WIN32 == 0
  { wait_status_t status;		/* UNIX */
    int child = fork();

    switch (child)
    { case -1:
         mse_cc = ENOSPACE;
         return false;
			/*NOTREACHED*/
      case 0:
	 close(0);	/*	we want reads to fail */
      /* close(1);	 but not writes or ioctl's */
      /* close(2); */
	(void) execvp(args[0], args);
	_exit(errno);
			/*NOTREACHED*/
    }
    dowait(&status);
    if (!WIFEXITED(status))
    { mse_cc = EERROR;
      return false;
    }	
    if (WIFSIGNALED(status))
    { mse_cc = EKILLED;
      return false;
    }	
    
    return WEXITSTATUS(status) == 0;
  }
#else
  { int	status = spawnvp(0, args[0], args);
    if (status != OK)
    { mse_cc = ENOSPACE;
      return false;
    }
    
    return true;
  }
#endif /* S_WIN32 */
}


extern char * getenv();

/*	SHELLPROG: Execute a command in a subshell		*/

short shellprog(cmd)
	 char *cmd;	/*  Incoming command line to execute  */
{	char comline[NSTRING];	/* constructed command line */
 	char swchar;
#if S_MSDOS == 0 || S_WIN32
        swchar = '/';
#else
		/*  detect current switch character and set us up to use it */
	rg.x.ax = 0x3700;	/*  get setting data  */
				/*  al: get switch character to use */
	intdos(&rg, &rg);
 	swchar = (char)rg.h.dl;
#endif
				/*  get name of system shell  */
{       char * shell = getenv("COMSPEC");
	if (shell == null)
	  return false;
	  
	while (*cmd == ' ' || *cmd == '\t')
	  cmd++;
		 /**  If the command line is not empty, bring up the shell  **/
		 /**  and execute the command.  Otherwise, bring up the     **/
		 /**  shell in interactive mode.   **/
	if (*cmd)
	{
	  strcat(strcat(strcpy(comline, shell), " ?c "), cmd);

	  comline[strlen(shell) + 1] = swchar;		/* replace the ? */
	  shell = comline;
	}
	return execprog(shell);
}}

# endif /* ! S_MSDOS */


