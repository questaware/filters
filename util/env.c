#include  <stdio.h>
#include  <signal.h>
#include  <termio.h>
#include  <fcntl.h>
#include  <errno.h>
#if NONDIR
# include  <sys/ndir.h>
#endif
#include  "build.h"

#include  "../h/tcap.h"
#if 0
# include  <stdlib.h>
# include  <sys/vid.h>
#endif
#include  <sys/types.h>
#include  <sys/stat.h>


#include "../common/h/eprintf.h"

Char *   this_ttype;

#define A_ESC (0x1b)

Int kbdpoll;			/* in O_NDELAY mode		*/
Int kbdflgs;			/* saved keyboard fd flags	*/

struct	termio	otermio;	/* original terminal characteristics */
struct	termio	otermio_;	/* original line characteristics */
struct  termio  ntermio;	
struct  termio  ntermio_;	

#define TCAPSLEN 1024
Char tcapbuf[TCAPSLEN];
Char *CM, *CE, *CL, *SO, *SE, *SR, *SF, *IS, *KS, *KE;

Bool screxist;
Bool vt100;
Bool ansi;

Char PREFIX[10]; /* not used, all keys are assumed to be ESC ? x */
Int  prefixlen;

Short keytbl[256];

Term_t term;

FILE * dbg_file = null; 


extern	unsigned char *tparm(), *tgoto(), *tigetstr(),
		   term_parm_err[], *term_err_strings[], *Def_term, *termname();
extern  Char * tgetstr();		   
extern	void  tputs(), putp(), vidputs(), vidattr();

private Char * strsrch(s, pat)
	 Char * s;
	 Char * pat;
{ Char pch = *pat;

  for (; *s != 0; ++s)
    if (*s == pch)
    { Char * pat_ = pat;
      Char * s_ = s;
      
      while (*s_ == *pat_)
      { if (*pat_ == 0)
          return s;
        ++s_; 
        ++pat_;
      }
    }

  return null;
}

public void ttopen(fildes)
      int   fildes;
{ struct termio * ot = &otermio;    
  Cc cc = ioctl(fildes, TCGETA, ot);	/* save old settings */
  if (cc != 0)
    i_log(cc,"Ioctl failed %d", errno);
    
  ntermio = *ot;
  ntermio.c_iflag = BRKINT;			/* setup new settings */
  ntermio.c_lflag = ISIG;
  ntermio.c_oflag = 0;
  ntermio.c_cc[VMIN]  = 1;
  ntermio.c_cc[VTIME] = 2;

  cc = ioctl(fildes, TCSETA, &ntermio);	/* and activate them */
  if (cc != 0)
    i_log(cc,"Ioctl failed");

  if (fildes == 0)
  { kbdflgs = fcntl(fildes, F_GETFL, 0);
    kbdpoll = false;
    setbuf(stdout, null);
  }
}


public void ttignbrk(fildes)
      int   fildes;
{ ntermio.c_iflag = 0;
  ntermio.c_lflag = 0;
{ Cc cc = ioctl(fildes, TCSETA, &ntermio);
  if (cc != 0)
    i_log(cc,"Ioctl failed");
}}



public void ttclose (fildes)
         int    fildes;
{ if (otermio.c_cflag != 0)
  { ioctl(fildes, TCSETA, &otermio); 		/* restore terminal settings */
    fcntl(fildes, F_SETFL, kbdflgs);
  }
}

#define putpad(str) tputs(str,1,ttputc)
#define putnpad(str, n) tputs(str,n,ttputc)
public void ttputc() forward;


public void tcapopen(termtype)        /* The initialisation routine */
        Char * termtype;
{ Char tcbuf[1024];

  if (termtype == NULL or tgetent(tcbuf, termtype) != 1) 
  { eprintf(&tcbuf[0], "Unknown terminal type %s!", termtype);
    exit( i_log(EERROR,tcbuf) );
  }
  vt100 = strsrch(termtype, "vt100") != null;
  ansi = strsrch(termtype, "ansi") != null;
 
  term.t_mrow = term.t_nrow = (short)tgetnum("li");
  if (term.t_nrow <= 0) 
    exit( i_log(EERROR, "termcap entry incomplete (lines)") );
  if (term.t_nrow == 24)
    term.t_nrow = 25;		/* we know best */

  term.t_mcol = term.t_ncol = (short)tgetnum("co");
  if (term.t_ncol <= 0)
    exit( i_log(EERROR, "Termcap entry incomplete (columns)") );
  
{ Char * p = tcapbuf;
  Char * homekey;
  CL = tgetstr("cl", &p);
  CM = tgetstr("cm", &p);
  CE = tgetstr("ce", &p); /* clear to end of line */
  SE = tgetstr("se", &p);
  SO = tgetstr("so", &p);
  if (vt100)
    SR = "\033[25A\033M";
  else
  { SR = tgetstr("SR", &p);
    if (SR == NULL)
      SR = tgetstr("sr", &p);
  }

  if (vt100)
    SF = "\033[25B\033D";
  else
  { SF = tgetstr("SF", &p);
    if (SF == NULL)
      SF = tgetstr("sf", &p);
  }

  screxist = SF != NULL && SR != NULL;
  IS = tgetstr("is", &p); /* extract init string */
  KS = tgetstr("ks", &p); /* extract keypad transmit string */
  KE = tgetstr("ke", &p); /* extract keypad transmit end string */

  if (CL == NULL or CM == NULL or CE == NULL)
    exit( i_log(EERROR, "Incomplete termcap entry\n") );

  prefixlen = 0;
{ Char * tgs = tgetstr("ku", &p);
  if (tgs != null)
  { prefixlen = strlen(tgs) - 1;
    keytbl[tgs[prefixlen]] = -'u';
    if (in_range(prefixlen, 1, 10))
      memcpy(PREFIX, tgs, prefixlen);
  }
  tgs = tgetstr("kd", &p);
  if (tgs != null)
    keytbl[tgs[prefixlen]] = -'d';
  tgs = tgetstr("kl", &p);
  if (tgs != null)
    keytbl[tgs[prefixlen]] = -'l';
  tgs = tgetstr("kr", &p);
  if (tgs != null)
    keytbl[tgs[prefixlen]] = -'r';
  tgs = tgetstr("PU", &p);
  if (tgs != null)
    keytbl[tgs[prefixlen]] = -'U';
  tgs = tgetstr("PD", &p);
  if (tgs != null)
    keytbl[tgs[prefixlen]] = -'D';
  homekey = tgetstr("kh", &p);
  if (homekey != null)
    keytbl[homekey[prefixlen]] = -'H';
  tgs = tgetstr("k1", &p);
  if (tgs != null and vt100 and homekey == null)
    keytbl[tgs[prefixlen]] = -'H';
  tgs = tgetstr("k2", &p);
  if (tgs != null and vt100 and homekey == null)
    keytbl[tgs[prefixlen]] = -'D';
  tgs = tgetstr("k3", &p);
  if (tgs != null and vt100 and homekey == null)
    keytbl[tgs[prefixlen]] = -'U';
   	        
  ttopen(fileno(stdin));
  if (p >= &tcapbuf[TCAPSLEN])
    exit( i_log(EERROR, "Terminal description too big!\n") );

  if (IS != NULL)			/* send init strings if defined */
    putpad(IS);
#if 0
  if (KS != NULL)
    putpad(KS);
#endif
}}}
 

public void tcapclose()

{ 
#if 0
  if (KE != NULL)		/* send end-of-keypad-transmit string */
    putpad(KE);
#endif
  ttclose(fileno(stdin)); /* surely 0 by definition ! */
}

public void ttputc(c)
	Char c;
{ write(1, &c, 1);
}


public void ttputs(s)
	Char * s;
{ while (*s != 0)
    write(1, s++, 1);
}
	  

Int ttgetc(fildes)
        Id   fildes;
{ char c;   /* must be signed */
/*Char * pfx = &PREFIX[0];   dont use PREFIX at present */
  read(fildes, &c, 1);  

  if (c < ' ')
    if      (c == A_ESC)
    { read(fildes, &c, 1);      /* O or [ */
      read(fildes, &c, 1);  
      return keytbl[ 255 & (int)c ];
    }
    else if ((c & 255) == 0x9b)		/* support vt arrow keys */
    { Char b;
      read(fildes, &c, 1);
      if (in_range(c, 'A', 'D'))
        return - "udrl"[ 255 & ((int)c - 'A')];
      else
      { read(fildes, &b, 1);
        return - "?HU??D" [ 255 & ((int)c - '1')];
      }
    }
    else if (c > 0 and keytbl[c] != 0)
      return keytbl[c];
      
  return c;
}



Int hold()

{ return ttgetc(0);
}

public void ttmove(row, col)
      register int row, col;
{
  putpad(tgoto(CM, col, row));
}


public void tteeol()

{ putpad(CE);
}


public void tteeop()

{ putpad(CL);
}

#if	COLOR

Char fcol[] = "\033[3 m\000";
Char bcol[] = "\033[4 m\000";

public void ttfcol(color)	/* no colors here, ignore this */

{ fcol[3] = color + '0';
  ttputs(fcol);
}

public void ttbcol(color)	/* no colors here, ignore this */

{ bcol[3] = color + '0';
  ttputs(bcol);
}
#endif

static Char attrbuf[40];
Short	cfcolor = 0x7;		/* current forground color */
Short	cbcolor = 0;		/* current background color */
Short	cmode = 0;
Short	ofcolor = 0x7;
Short	obcolor = 0;

public void ttrev(state)		/* uses ansi modes */
	int  state;
{ if ((state & 0xffff) == 0xffff)
  { ttputs("\033[0m");
    cfcolor = ofcolor;
    cbcolor = obcolor;
    cmode = 0;
  }
  else
  {      Short fcol = state & 0xf;
         Short bcol = (state & 0xf0) >> 4;
         Short mode = state & 0xf00;
    fast Char * p = &attrbuf[1];
    
    attrbuf[0] = A_ESC;
    if (mode != cmode)
    { if (mode & A_BOLD)
      { *p++ = ';'; *p++ = '1';
      }
      if (mode & A_USCORE)
      { *p++ = ';'; *p++ = '4';
      }
      if (mode & A_BLINK)
      { *p++ = ';'; *p++ = '5';
      }
      if (mode & A_REV)
      { *p++ = ';'; *p++ = '7';
      }
      cmode = mode;
    }
    if (fcol != bcol)
    { if (fcol != cfcolor)
      { *p++ = ';'; *p++ = '3';
        *p++ = '0'+fcol;
        cfcolor = fcol;
      }
      if (bcol != cbcolor)
      { *p++ = ';'; *p++ = '4';
        *p++ = '0'+bcol;
        cbcolor = bcol;
      }
    }
    *p++ = 'm';
    *p++ = 0;
    attrbuf[1] = '[';
    ttputs(attrbuf);
  }
}

public void ttbeep()

{ ttputc(A_BEL);
}


public void ttscdn()

{ ttmove(term.t_nrow-1, 0);
  putnpad(SF, 25);
}


public void ttscup()

{ ttmove(0,0);
  putnpad(SR, 25);
}


public Bool ttcansc()

{ return screxist;
}

struct stat fszstat;
static Cc examf_cc;

public Cc examine_fildes(fildes)
	Fildes fildes;
{ examf_cc = fstat(fildes, &fszstat);
  return examf_cc;
}



public Int this_fsize()

{ return examf_cc != OK ? 0 : fszstat.st_size;
}



public Int this_moddate()

{ return examf_cc != OK ? 0 : fszstat.st_mtime;
}
