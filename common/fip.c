/*	FIP.C:	Operating specific I/O mainipulating streams
*/

#include	<stdio.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	"build.h"

#if	S_UNIX5 | HPUX | SUN | XENIX			/* System V */
#if S_BORLAND == 0
#  include	<termio.h>
#  include	<fcntl.h>
#endif
#endif

  static int  kbdflgs;			/* saved keyboard fd flags	*/
  static int  kbdpoll;			/* in O_NDELAY mode		*/
  static int  kbdqp;			/* there is a char in kbdq	*/
  static char kbdq;			/* char we've already read	*/

#if  S_MSDOS
#include <dos.h>
 extern union REGS rg;		/* cpu register for use of DOS calls */
 extern struct SREGS segreg;	/* cpu segment registers	     */
#else
 #include <stdlib.h>
 #include <signal.h>
 #if S_BORLAND == 0
  static struct	termio	otermio;	/* original terminal characteristics */
  static struct	termio	ntermio;	/* charactoristics to use inside */
 #endif
#endif

extern int tidyup();
extern void serialclose();

					/* returns old input */
FILE * splitip()

{ struct stat fstat_;
  int cc = fstat(0 , &fstat_);
  if (fstat_.st_mode & 020000)
    return 0;

{ short id = dup(0);
  if (id < 0)
    return null;

  close(0);
{
 #if S_MSDOS
  int ip = open("CON", O_RDONLY+O_BINARY);
 #else
  int ip = open("/dev/tty", O_RDONLY);
 #endif
  if (ip < 0)
    return null;
  
  return fdopen(id, "r");
}}}

#if S_MSDOS == 0 

Bool serial_is_open = false;

void serialopen()

{
#if S_BORLAND == 0
#if	S_UNIX5 | S_HPUX | S_SUN | S_XENIX
	ioctl(0, TCGETA, &otermio);	/* save old settings */
#if 0
	ntermio.c_iflag = 0;		/* setup new settings */
	ntermio.c_oflag = 0;
	ntermio.c_lflag = 0;
	ntermio.c_cc[VTIME] = 0;
#endif
	ntermio.c_cflag = otermio.c_cflag;
	ntermio.c_line = otermio.c_line;
	ntermio.c_cc[VMIN] = 1;
	ioctl(0, TCSETAW, &ntermio);	/* and activate them */
	kbdflgs = fcntl( 0, F_GETFL, 0 );
	kbdpoll = false;
#elif    S_BSD
        gtty(0, &ostate);                       /* save old state */
        gtty(0, &nstate);                       /* get base of new state */
        nstate.sg_flags |= RAW;
        nstate.sg_flags &= ~(ECHO|CRMOD);       /* no echo for now... */
        stty(0, &nstate);                       /* set mode */
	ioctl(0, TIOCGETC, &otchars);		/* Save old characters */
	ioctl(0, TIOCSETC, &ntchars);		/* Place new character into K */
			/* provide a smaller terminal output buffer so that
	   	   	   the type ahead detection works better (more often) */
	setbuffer(stdout, &tobuf[0], TBUFSIZ);
	signal(SIGTSTP,SIG_DFL);	/* set signals so that we can */
	signal(SIGCONT,rtfrmshell);	/* suspend & restart emacs */
#endif
#if S_MSDOS == 0 && S_BORLAND == 0
	serial_is_open = true;
#endif
#endif
/*	signal(SIGTERM,(void*)tidyup); */
		/* we are not sure of the initial position of the cursor */
}

			/* This function gets called just before we go
			 * back home to the command interpreter.	 */
void serialclose()

{
#if S_MSDOS == 0 && S_BORLAND == 0
    if (serial_is_open)
    {
#if	S_UNIX5 | HPUX | SUN | XENIX
	ioctl(0, TCSETA, &otermio);	/* restore terminal settings */
	fcntl(0, F_SETFL, kbdflgs);
#endif

#if     V7 | S_BSD
        stty(0, &ostate);
	ioctl(0, TIOCSETC, &otchars);	/* Place old character into K */
#endif
    }
    serial_is_open = false;
#endif
}

#endif

#if S_MSDOS || S_BORLAND
#define seropen()
#define serclose()

#else
#define seropen() if (not serial_is_open) \
                    serialopen();
#define serclose() if (serial_is_open) \
                    serialclose();

#endif

Short grabchar()				/* direct from terminal */

{
	seropen();
{
#if S_MSDOS && S_WIN32 == 0
	rg.h.ah = 7;		/* dos Direct Console Input call */
	intdos(&rg, &rg);
	if (rg.h.al == 0)	/* function key!! */
	{ rg.h.ah = 7;		/* get the next character */
	  intdos(&rg, &rg);
	  rg.h.al = -rg.h.al;
	}
        serclose();
	return rg.h.al & 255;
#elif S_BORLAND
  char buf[88];			/* cannot make single char get work */
  fgets(buf, sizeof(buf), stdin);
  serclose();
  return buf[0];
#else
  char ch;
  if (read(0, &ch, 1) <= 0)
    ch = -1;
  serclose();
  return ch;
#endif
}}


	/* typahead:	See if any characters are already in the keyboard buffer
	*/
#if S_MSDOS && S_WIN32 == 0 && S_BORLAND == 0

Bool typahead()

{	rg.x.ax = 0x4406;	/* IOCTL input status */
	rg.x.bx = 0;		/* File handle = stdin */
#if	MSC | DTL
	int86(0x21,&rg,&rg);
	return rg.h.al != 0;
#elif	LATTICE | AZTEC | TURBO
	return intdos(&rg, &rg) != 0;
#else
	intcall(&rg, &rg, 0x21);
	return rg.x.flags != 0;
#endif
}

#endif
