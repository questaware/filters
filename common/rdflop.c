#include        <stdio.h>
#include        <string.h>
#include        <fcntl.h>
#include 	<sys/types.h>
#include 	<sys/stat.h>
#include        <errno.h>
#include        "build.h"
#if S_MSDOS
#include        <dos.h>
#include        <io.h>
#endif
#include        "h/rdmsflop.h"
#include        "h/eprintf.h"



#define S_DOSONLY (S_MSDOS && S_WIN32 == 0)

#if S_BORLAND
#define FBUFF 0
#else
#define FBUFF 1
#endif

#define SECTSZ 512
#define FLEN_FLD 10


#if S_DOSONLY

#define FLOP_FAC (0x13)

#define DTR_250 2
#define dtr(drv) (drv == 0 ? (((*(char*)0x490) >> 6) & 3) \
			   : (((*(char*)0x491) >> 6) & 3))

#define SECTSPERTRACK 9

char trackbuf[SECTSZ * SECTSPERTRACK];

char last_op;

typedef struct
{ Byte  intro;
  Byte  rwspecfunc;
  Short rwhead;
  Short rwcylinder;
  Short rwfirstsector;
  Short rwnoosectors;
  Int   rwbuffer;
} Rwbuf_t, *Rwbuf;

#if 0

typedef struct
{ Byte   dpspecfunc;
  Byte   dpdevtype;
  Short  dpdevattr;
  Short  dpcylinders;
  Byte   dpmediatype;
  Byte   dp0bytespersec;
  Byte   dp1bytespersec;
  Byte   dpsecperclust;
  Short  dpressectors;
  Byte   dpfats;
  Byte   dp0rootdirents;
  Byte   dp1rootdirents;
  Byte   dp0sectors;
  Byte   dp1sectors;
  Byte   dpmedia;
  Short  dpfatsecs;
  Short  dpsecspertrack;
  Short  dpheads;
  Int    dphiddensecs;
  Int    dphugesectors;
} Dpstruct_t, *Dpstruct;

#endif

Short      this_heads;
Short      this_cyls;
Short      this_sects;
Rwbuf_t    rwbuf_t;


 union REGS rg;		/* cpu register for use of DOS calls */
 struct SREGS segreg;	/* cpu segment registers	     */

#endif

Short      this_drv = -1;

Bool  rdx_has_vols = false;

Short this_chan = -1;

#if FBUFF

 FILE * this_fdcr;

#endif

#if FBUFF
#define myseek(offs, wh) fseek(this_fdcr, offs, wh)
#define myread(t, ct) fread(t, 1, ct, this_fdcr)
#define mywrite(s,ct) fwrite(s, 1, ct, this_fdcr)
#else
#define myseek(offs, wh) lseek(this_chan, offs, wh)
#define myread(t, ct) read(this_chan, t, ct)
#define mywrite(s,ct) write(this_chan, s, ct)
#endif

Cc rdx_rw(Char * t, Short wh) forward;

#if S_DOSONLY

doflop()

{ rg.x.dx = this_drv;
  rg.h.dh = rwbuf_t.rwhead;
  int86(FLOP_FAC, &rg, &rg);
}

static Cc rdf_init(Boool rw, Vint cyls, Char * s)

{ Char buf[512];
  this_drv = s[1] != ':' ? -1 : toupper(*s) - 'A';
  if (not in_range(this_drv, 0, 'Z' - 'A'))
    this_drv = -1;

  rg.x.ax = (FLOP_RESET << 8);
  doflop();
  if (rg.x.cflag != OK)
    return NOT_FOUND;
#if 0
  rg.x.ax = (FLOP_RECAL << 8);
  doflop();
  if (rg.x.cflag != OK)
    return NOT_FOUND;
#endif
  rg.x.ax = (FLOP_READ_CMOS << 8);
  doflop();
  if (rg.x.cflag != OK)
    return NOT_FOUND;

  this_heads = 2;
  this_cyls =  rg.h.ch + 1;
  this_sects = cyls != 0 ? cyls : rg.h.cl;
			/* dps_t.dp0bytespersec + 256 * dps_t.dp0bytespersec; */
  rwbuf_t.rwfirstsector = -1;
  rwbuf_t.rwhead	= 0;
  rwbuf_t.rwcylinder    = 0;
  rwbuf_t.rwnoosectors  = this_heads * this_cyls * this_sects;
  rdf_rw(&buf[0], FLOP_RD);
#if 1
  printf( "HCS %d %d %d -- %d\n", 
			this_heads, this_cyls, this_sects, 
			rwbuf_t.rwnoosectors);
#endif
  rwbuf_t.rwhead	= 0;
  rwbuf_t.rwfirstsector = -1;
#if 1
{ Char * dpt = *(Char**)0x78;
  if (not rw)
    dpt[0] = 0xae;
    
  dpt[0x9] = not rw ? 0 : 8;
}
#endif
  return OK;
}


#endif

void rdf_fini()

{ 
#if S_DOSONLY
  Char buf[SECTSZ];
  if (last_op == FLOP_WR and this_drv >= 0)
    while (rwbuf_t.rwfirstsector != 0)
    { /* printf("FILL"); */
      memset(&buf[0], 0, SECTSZ);
      rdf_rw(&buf[0], FLOP_WR);
    }
#endif
}

Cc rdf_writespace()

{
#if S_DOSONLY
   return ((this_cyls - rwbuf_t.rwcylinder) * this_heads +
	  (this_heads - rwbuf_t.rwhead)) * this_sects +
	  (this_sects - rwbuf_t.rwfirstsector);
#else
   return 0x7f00;  /* say */
#endif
}


Cc rdf_diskspace()

{
#if S_DOSONLY
  return this_cyls * this_heads * this_sects;
#else
  return 0x7f00;  /* say */
#endif
}


static Cc rdf_rw(Char * t, Short wh)

{
#if S_DOSONLY
  rwbuf_t.rwfirstsector += 1;
  if (rwbuf_t.rwfirstsector >= this_sects)
  { rwbuf_t.rwhead += rwbuf_t.rwfirstsector / this_sects;
    rwbuf_t.rwfirstsector = 0;
    if (rwbuf_t.rwhead >= this_heads)
    { rwbuf_t.rwhead = 0;
      rwbuf_t.rwcylinder += 1;
      if (rwbuf_t.rwcylinder >= this_cyls)
        return HALTED;
    }
  }

{ Int sect = rwbuf_t.rwfirstsector * SECTSZ;
  if (sect >= SECTSPERTRACK * SECTSZ)
    sect -= SECTSPERTRACK * SECTSZ;

  last_op = wh;
  if (last_op == FLOP_WR)
  { memcpy(&trackbuf[sect], t, SECTSZ);
    if (sect < (SECTSPERTRACK - 1) * SECTSZ)
      return OK;
  }
  else
  { if (sect != 0)
    { memcpy(&t[0], &trackbuf[sect], SECTSZ);
      return OK;
    }
  }
  
#if 0
  printf( "_ACT_ %d ", rwbuf_t.rwfirstsector);
#endif
  rwbuf_t.rwbuffer = (Int)&trackbuf[0];

{ Short retry = 3;

  while (--retry >= 0)  
  { int clamp = 30000;
    while (--clamp != 0)		/* sleep */
    { rg.x.ax = (FLOP_READY << 8);
      doflop();
      if (rg.x.cflag == 0)
        break;
    }

    rg.x.ax = (wh << 8) + SECTSPERTRACK;
    rg.h.ch = rwbuf_t.rwcylinder;
    rg.h.cl = rwbuf_t.rwfirstsector < SECTSPERTRACK ? 1 : SECTSPERTRACK + 1;
    rg.x.bx = (unsigned int)&trackbuf[0];
    segreg.es = ((unsigned long)&trackbuf[0]) >> 16;
#if 0
    printf( "P %x %x %x %x\n", rg.x.ax, rg.x.bx, rg.x.cx, rg.x.dx);
#endif
    doflop();
/*  printf("R/W (%x)\n", rg.x.cflag); */
    if (rg.x.cflag == 0)
    { int dr = dtr(this_drv);
      if (dr == DTR_250 and this_sects == 18)
      { this_sects = 9;
        rwbuf_t.rwnoosectors  = this_heads * this_cyls * this_sects;
#if 1
        printf( "%d NCS %d %d %d -- %d\n", dr,
			this_heads, this_cyls, this_sects, 
			rwbuf_t.rwnoosectors);
#endif
      }
      if (wh == FLOP_RD)
        memcpy(&t[0], &trackbuf[0], SECTSZ);
      return OK;
    }
#if 1
    fprintf(stderr, "NOT YET (%x)\n", rg.x.ax);
#endif
    rg.x.ax = (FLOP_RESET << 8);
    doflop();
#if 0
    rg.x.ax = (FLOP_RECAL << 8);
    doflop();
#endif
  }

  fprintf(stderr, "Drive %c failed (%d)\n", this_drv+'A', rg.x.ax);
  return rg.x.cflag;
}}
#else
  return OK;
#endif
}



Cc rdf_read(Char * t)

{ 
#if S_DOSONLY
  return rdf_rw(t, FLOP_RD);
#else
  return OK;
#endif
}



Cc rdf_write(Char * t)

{
#if S_DOSONLY
  return rdf_rw(t, FLOP_WR);
#else
  return OK;
#endif
}

Int app_offs;

Cc rdx_init(Bool rw, Bool app, Bool tl_opt, Vint cyls, Char * s)

{
#if FBUFF == 0
  Short props = (not rw  ? O_RDONLY : 
                 not app ? O_RDWR+O_CREAT+O_TRUNC :
                           O_RDWR+O_APPEND);
#else
  Char * props = not rw ? "rb"  :
                 app    ? "a+b" : "wb";
#endif
  Bool is_std = strcmp(s, "-") == 0;
#if S_DOSONLY
  if (isalpha(s[0]) and s[1] == ':' and s[2] == 0)
  { rdx_has_vols = true;
    return rdf_init(rw, cyls, s);
  }
#endif

  #if FBUFF == 0
    this_chan = is_std ? 
                  not rw ? 0 : 1
                       : not rw ? open(s, props | O_BINARY)
                                : open(s, props | O_BINARY, S_IREAD|S_IWRITE);
  #else
    this_chan = 100;
  /*fprintf(stderr,"Fopen %s %s\n", s, props);*/
    this_fdcr = is_std ? 
                  not rw ? stdin : stdout
    		       : fopen(s, props);
    if (this_fdcr == null)
      this_chan = -1;
    else
      this_chan = fileno(this_fdcr);
  #endif

  app_offs = not rw or not app ? 0
#if FBUFF == 0
    			       : lseek(this_chan, 0, SEEK_END);
#else
    			       : ftell(this_fdcr);
#endif
  if (not rw and tl_opt and this_chan >= 0)
  { Char len_fld[FLEN_FLD+1];
    Vint ix;
    myseek(-(long)FLEN_FLD, SEEK_END);
      
    len_fld[FLEN_FLD] = 0;
    myread(&len_fld[0], FLEN_FLD);
    for (ix = FLEN_FLD; --ix >= 0 and len_fld[ix] != 'S'; )
      ;
      
    myseek(atol(&len_fld[ix+1]), SEEK_SET);
  }
  return this_chan >= 0 ? OK : NOT_FOUND;  
}



void rdx_term(Vint termblks)

{ Char buf[SECTSZ];
  while (--termblks >= 0)
  { memset(&buf[0], 0, SECTSZ);
    rdx_rw(&buf[0], FLOP_WR);
  }
}


void rdx_fini(Bool tl_opt)

{ if (tl_opt)
  { Char nbuf[20];
    sprintf(&nbuf[0], "S%ld", app_offs);
  { Short ix;
#if FBUFF && ! S_DOSONLY
    fwrite(&nbuf[0], strlen(nbuf), 1, this_fdcr);
#else
    for (ix = -1; ++ix < strlen(nbuf);)
      write(this_chan, &nbuf[ix], 1);
#endif
  }}
  
#if FBUFF
  if (this_fdcr != null)
    fclose(this_fdcr);
#else
  if (this_chan >= 0)
    close(this_chan);
#endif
  this_chan = -1;
}



Cc rdx_rw(Char * t, Short wh)

{ if (this_chan < 0)
    return rdf_rw(t, wh);
{ Short ct = SECTSZ;
  fast Short actual;
/*fprintf(stderr, "Bef this_fdcr %lx\n", this_fdcr);*/

  while (ct > 0 and (actual = wh == FLOP_RD ? myread(&t[0], 1)
					    : mywrite(&t[0], ct)) > 0)
  { t += actual;
    ct -= actual;
  }

/*fprintf(stderr, "rdx_rw: actual %d this_fdcr %lx\n", actual, this_fdcr);*/

  return ct == 0		       ? OK     : 
	 wh == FLOP_WR		       ? errno  :
	 actual == 0 or errno == EBADF ? HALTED : errno;
}}

