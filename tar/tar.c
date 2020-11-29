#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
/*#include <dir.h>*/
#include <io.h>
#include <errno.h>
#include <signal.h>
#include "build.h"
#include "../../util/h/dict.h"
#include "../../util/h/strutil.h"
/*#include "../../util/h/malloc.h"*/
#include "../../common/h/rdmsflop.h"

#if S_VMS == 0
#include "../../common/h/msdir.h"
typedef Int FStream;
#else
#include         <stdio.h>
#include         <stdlib.h>
#include         <dirent.h>
#include         <unistd.h>
typedef FILE * FStream;
#define DIRENT dirent

#include ssdef
#include descrip
#include jpidef
#include iodef
#include rms
#include starlet
#include libdef
#include lib$routines

#endif

int g_flen;

char * fline;
int    flinelen;
int    fline_ix;

#define FIOFNF 1
#define FIOSUC 0
#define FIOERR -1
#define FIOMEM -1
#define FIOEOF 2


#define S_DOSONLY S_MSDOS && S_WIN32 == 0
#define S_UNIXTEXT (S_MSDOS == 0 && S_WIN32 == 0)

#if S_MSDOS
#if S_WIN32 == 0
error error
#include <bios.h>
#endif
#endif


/*extern Char * malloc();*/

extern Bool rdx_has_vols;



#if S_MSDOS || S_WIN32
#define O_MY_BINARY O_BINARY
#else
#define O_MY_BINARY 0
#endif


#undef VERSION
#define VERSION "Version 1.01"

#define LOG_FS_BLKSZ 9

#define TBLOCK 512
#define NBLOCK  20
#define NAMSIZ 100

typedef union 
{ char little[sizeof(long)];
  long big;
} Prec_t;

Prec_t lend;

#define AM_LITTLEENDIAN (lend.little[0] != 0)

Int switch_big(Int x)

{ if (AM_LITTLEENDIAN)
    return x;

{ Prec_t prec; prec.big = x;
{ char i = prec.little[0];
  prec.little[0] = prec.little[3];
  prec.little[3] = i;
       i = prec.little[1];
  prec.little[1] = prec.little[2];
  prec.little[2] = i;
  return  prec.big;
}}}


Int switch_short(Short x)

{ if (AM_LITTLEENDIAN)
    return x;
  return ((x << 8) + ((x >> 8) & 0xff));
}




typedef struct
{ Char  name[NAMSIZ];
  Char  mode[8];
  Char  uid[8];
  Char  gid[8];
  Char  size[12];
  Char  mtime[12];
  Char  chksum[8];
  Char  linkflag;
  Char  linkname[NAMSIZ];
  Char  extno[4];
  Char  extotal[4];
  Char  efsize[12];  
} Tardcr_t, *Tardcr;

#define M_LINKED 0x8000

typedef struct
{ Short mode;				/* all always little endian */
  Int   size;
  Int   mtime;
  Char  name_linkname[TBLOCK - 10];
} Stardcr_t, *Stardcr;

union
{ Tardcr_t  t;
  Stardcr_t s;
  Char      b[TBLOCK+2];
} tardcr;

Char  last_name_linkname[256 - 17];

typedef struct
{ Char  name[NAMSIZ+1];
  Short mode;
  Short uid;
  Short gid;
  Int   size;
  Int   mtime;
  Short chksum;
  Short linkflag;
  Char  linkname[NAMSIZ];
  int   headtext;
/*
  Char  extno[4];
  Char  extotal[4];
  Int   efsize;  
*/
} Idcr_t, *Idcr;

Idcr_t  idcr_t;

Char  failname[NAMSIZ+1];

Char block[TBLOCK+1];

#define MAXLN 133

Char line[MAXLN+1];

Dict dict;
Dict idict;

Int flen = -1;
Short blk_ct = 0;
FStream ipfile;


int this_file;
Cc this_cc = OK;
Bool qualified = false;			/* only certain files wanted */
Short userno = 0;
Short groupno = 0;

Char * devnm = "/dev/install";
Char * passdir = null;

  Bool a_opt = false;
  Bool t_opt = false;
  Bool k_opt = false;
  Bool c_opt = false;
  Bool s_opt = false;
  Bool tl_opt = false;
  Bool v_opt = false;
  Bool x_opt = false;
  Bool y_opt = false;
  Bool z_opt = false;
  Bool compress_opt = false;
  Bool strict_opt = true;
  Bool sio_opt = false;
  Vint flcyls = 0;
  Int vol_sz = 0;
  Vint termblks;

#if 0

printf()
{ fputs("(f)printf() called", stderr);
}

fprintf()
{ printf();
}

#endif


struct stat fstat_;

Bool name_is_diry( Char * s)

{
#if S_MSDOS
  return stat(s, &fstat_) == OK and (fstat_.st_mode & S_IFMT) == S_IFDIR;
#elif S_VMS
  return stat(s, &fstat_) == OK and (fstat_.st_mode & S_IFMT) == S_IFDIR;
#else
  return lstat(s, &fstat_) == OK and (fstat_.st_mode & S_IFMT) == S_IFDIR;
#endif
}


Bool name_is_datafile(Char * s)

{ return !name_is_diry and (fstat_.st_mode & S_IFREG) != 0;
}



static void explain()

{ fprintf(stderr, "tar {options} {-f dev} { file }*\n"
		  "  -=     -- Version number\n"
		  "  -a     -- Autoconvert files to/from MSDOS\n"
		  "  -144   -- Force the floppy to be 1.44MB\n"
		  "  -. cmd -- Issue initial cmd, e.g. to chdir\n"
		  "  -c     -- Create the archive on the device\n"
		  "  -t     -- Simply display file names\n"
		  "  -x     -- Extract the files from the device\n"
		  "  -p dir -- copy files to directory\n"
		  "  -k     -- Preserve files already existing\n"
		  "  -s     -- Avoid splitting files over volumes\n"
		  "  -B #   -- Volume size in blocks\n"
                  "  --     -- EOF contains start point\n"
		  "  -u #   -- Created files to be given user no #\n"
		  "  -g #   -- Created files to be given group no #\n"
		  "  -i	    -- Allow headers with bad checksums\n"
		  "  -l	#   -- recordlength\n"
                  "  -b	    -- abbreviate file headers (proprietary))\n"
                  "  -r     -- Append ouptput to file\n"
		  "  -z #   -- number of zero blocks to append\n"
		  "  -v     -- Verbose mode\n\n");
  fprintf(stderr, "  where the files are { file } * or - => names from stdin\n"
		  "	       dev is  file or device or - => stdout\n");
}



Short calc_cs(Byte * s)

{ int bcs = 0;
  int i;
  for (i = TBLOCK; --i >= 0;)
    bcs += *s++;
  return bcs;
}



static Char * skip_dotslash(char * s)

{ if (s[0] == '.' and (s[1] == '/' or s[1] == '\\'))
    for ( ;(++s)[0] == '/' or s[0] == '\\';)
      ;
  return s;
}



static Cc ensure_path(char * s)
	 /* Char *   s;		** input; output: failure point */
{ fast int six;
  for (six = strlen(s);
       s[--six] != '/' and s[six] != '\\'; )
    if (six < 0)
      return OK;

  if (isalpha(s[0]) and s[1] == ':' and six == 2)
    return OK;

  six = 0;
{ Vint iter = 0;
  
  while (true)
  { if (s[six-1] != ':' and six != 0)
    { Char sch = s[six];
      s[six] = 0;
    { struct stat fstat_;
      Cc cc = stat(s, &fstat_);
      if (cc < 0 and errno == ENOENT)
#if S_WIN32
        cc = mkdir(s);
#else
        cc = mkdir(s, 0755);
#endif
      s[six] = sch;
      if (cc != OK)
        if (iter == 0)
          iter = 1;
        else
          return cc;
    }}
    for (; s[++six] != 0 and s[six] != '/' and s[six] != '\\';)
      ;
    if (s[six] == 0)
      break;

  }
  return OK;
}}



#if S_MSDOS == 0 || S_WIN32
# define filename_to_dos(s)
#else

static void filename_to_dos(char * s)
	 /* Char *   s;		** input; output */
{      char buf[FILENAMESZ+1];
  
       int len = strlen(s);
  fast int six;
       int noo_chr = 0;
       int noo_dir = 0;
       int dot_pos = 0;
       int dot_pos_end = 0;
       int tail = 0;
       Bool got_chg = false;
       
       if (y_opt or t_opt and not v_opt)
         return;
         
       if (len >= FILENAMESZ)
         len = FILENAMESZ;

       buf[len+1] = 0;
       
  for (six = len; ; )
  { --six;
    buf[six+1] = s[six];
    if (six < 0 or buf[six+1] == '/' or buf[six+1] == '\\')
    { if (buf[six+2] == '.' and noo_chr > 2)
      { buf[six+2] = '_';
	got_chg = true;
      }
      if (noo_dir == 0 and (got_chg or noo_chr > 8 + (dot_pos == 0 ? 0 : 4)))
      { tail = six;
        dot_pos_end = dot_pos;
      }

      noo_chr = -1;
      dot_pos = 0;
      noo_dir += 1;
      if (six < 0)
        break;
    }
    noo_chr += 1;
    if (buf[six+1] == '.')
    { if (dot_pos == 0 and noo_chr <= 4)
        dot_pos = six;
      else
      { buf[six+1] = '_';
	got_chg = true;
      }
    }
  }

  if (tail != 0)
  { int clamp = 4;
    int dpe = dot_pos_end;
    int trunc = tail+9;
  
    while (--clamp >= 0)
    { if (dot_pos_end != 0 and trunc > dpe)
        trunc = dpe;
      
      memmove(&buf[trunc+6], &buf[trunc+1], len <= trunc ? 12 : len + 1 - trunc);
      if (dot_pos_end == 0)
	buf[trunc+1] = 0;
      else
	strcpy(&buf[trunc+1], &s[dot_pos_end]);
    { static int clashct = 0;
      Key_t k_t;
      k_t._str = &buf[1];
      if (dict_read(idict, Q_STR, k_t) == null)
	break;
      memmove(&buf[trunc+1], ".~~~/", 5);
      buf[trunc+4] = 'a'+clashct++;
      if (clashct > 26)
        clashct = 0;
      got_chg = true;
      dpe += 5;
      trunc += 13;
      if (trunc > sizeof(idcr_t.name))
        break;
    }}
  }
{ Key_t k_t;
  k_t._str = (char*)malloc(strlen(&buf[1])+1);

  if (k_t._str != null)
  { strcpy(k_t._str, &buf[1]);
    dict_add(idict, Q_STR, k_t);
    if (was_there)
      fprintf(stdout,"Name clash %s\n", &buf[1]);
  }

  if (got_chg and v_opt)
    fprintf(stdout,"%s ->\n  %s\n", s, &buf[1]);

  strcpy(&s[0], &buf[1]);
}}

#endif

Short is_unix_text;		/* 0 => undecided, +ve => it is */
Bool  is_first_blk;


static void scan_init()

{ is_unix_text = 0;
  is_first_blk = true;
}



#define TR_BLK_SZ 1024

#if S_VMS
int tr_blk_sz = TR_BLK_SZ;
#endif

Byte trblk[TR_BLK_SZ];

static Short scan_block(Byte blk[512], int sz)

{ if (is_first_blk)
  { Byte * s;
    for (s = &blk[sz]; --s >= blk; )
    { if (*s < 7 or *s >= 128)
      { is_unix_text = S_UNIXTEXT ? 1 : -1;		/* same as the host */
        break;
      }
      if (*s == 10 and s != blk)
      { Bool is_u_t = s[-1] != 13 ? 1 : -1;
        if      (is_unix_text == 0)
          is_unix_text = is_u_t;
        else if (is_u_t != is_unix_text)
        { is_unix_text = S_UNIXTEXT ? 1 : -1;		/* same as the host */
	  break;
        }
      }
    }
    is_first_blk = false;
  }
  if (S_UNIXTEXT and is_unix_text > 0)
  { Byte * t = trblk-1;
    Byte * s;
    for (s = &blk[-1]; ++s < &blk[sz]; )
    { if (*s == 10)
        *++t = 13;
      *++t = *s;
    }
    
  /*eprintf(null, "ISU\n");*/
    return t - trblk-1;
  }
  if (not S_UNIXTEXT and is_unix_text < 0)
  { Byte * t = &trblk[-1];
    Byte * s;
    for (s = &blk[-1]; ++s < &blk[sz]; )
    { if (*s == 13)
        continue;
      *++t = *s;
    }
  /*eprintf(null, "NOTU\n");*/
    return t - trblk-1;    
  }
  
/*eprintf(null, "SU\n");*/
  return -1;
}

#if 1

char * utovmsfn(char * t_, char * s_)

{ static char buf[200];
  char * t = t_;
  char * s = s_;
  char * lt;
  
  for (lt = s_-1; *++lt != 0 && *lt != ':'; )
    ;

  
  if (*lt == ':')
  { memcpy(t_, s_, lt-s_);
    t_[            lt-s_] = 0;
    t = getenv(t_);
    if (t == NULL)
      t = t_;
    else
    { strcpy(t_, t);
      t = t_ + strlen(t_);
      s = lt+1;
    }
  }

  for (lt = s-1; *++lt != 0 && *lt != '/'; )
    ;

  if (lt[0] == 0)
  { strcpy(t,s);
  }
  else
  { s = s_;
    
    if (t > t_ && *(t-1) == ']')
     *(t-1) = '.';
    else
     *t++ = '[';
  
    while (s[0] == '.' && (s[1] == '.' && s[2] == '/'|| s[1] == '/'))
    { if (s[1] != '.') 
        s += 2;
      else
      { *t++ = '-';
        s += 3;
      }
    }
    
    lt = t;
    *t++ = '.';

    while (s[0] != 0 && (isalpha(s[0]) || isdigit(s[0])))
    { while (*s != 0 && *s != '/')
        *t++ = *s++;
      if (*s == 0) break;
      ++s;
      lt = t;
      *t++ = '.';
    }
    
    *lt = ']';
    *t++ = 0; 
  }
#if 0
  strcpy(buf,"FN=");
  strcat(buf,t_);
  putenv(buf);
#endif
#if S_VMS
  for (t = t_-1; *++t > 13; )
    ;
  *t = 0;
#endif
/*printf("UtV:%s\n", t_);*/
  return t_;    
}


#if S_VMS

/*
 * Here are the much faster I/O routines.  Skip the C stuff, use
 * the VMS I/O calls.  Puts the files in standard VMS format, too.
 */
#define successful(s)	((s) & 1)
#define unsuccessful(s) (!((s) & 1))

static struct FAB fab;		/* a file access block */
static struct RAB rab;		/* a record access block */

			    /* Open a file for reading. */
int ffropen(char *fn)
{
        unsigned long status;
        char vmsfn[1000];
				/* initialize structures */
	fab=cc$rms_fab;
	rab=cc$rms_rab;

	fab.fab$l_fna = utovmsfn(vmsfn,fn);
	fab.fab$b_fns = strlen(fab.fab$l_fna);
	fab.fab$b_fac = FAB$M_GET;
	fab.fab$b_shr = FAB$M_SHRGET;
	fab.fab$l_fop = FAB$M_SQO;

	rab.rab$l_fab = &fab;
	rab.rab$l_rop = RAB$M_RAH;	/* read-ahead for multibuffering */

        flinelen = -1;
        fline_ix = 0;

	status=SYS$OPEN(&fab);
	if (status==RMS$_FLK)
	{  /* File locking problem:
	    * Add the SHRPUT option, allowing shareability with other writers.
	    * This lets us read batch logs and stuff like that. Don't turn it on
	    * automatically since adding this sharing eliminates the read-ahead
	    */
		fab.fab$b_shr |= FAB$M_SHRPUT;
		status=SYS$OPEN(&fab);
	}

	if (successful(status))
	{	if (unsuccessful(SYS$CONNECT(&rab)))
		{
			SYS$CLOSE(&fab);
			return FIOFNF;
		}
	}
	else return FIOFNF;

        return FIOSUC;
}

/*
 * ffwopen(char *fn, char *mode)
 *
 * fn = file name, mode = 1 must not exist
 */
int ffwopen(int mode, char *fn)
{
	unsigned long status;
	char vmsfn[1000];

	/* initialize structures */
	fab=cc$rms_fab;
	rab=cc$rms_rab;

	fab.fab$l_fna = utovmsfn(vmsfn,fn);
	fab.fab$b_fns = strlen(fab.fab$l_fna);
	fab.fab$b_fac = FAB$M_PUT;	/* writing this file */
	fab.fab$b_shr = FAB$M_NIL;	/* no other writers */
	fab.fab$l_fop = FAB$M_SQO;	/* sequential ops only */
	fab.fab$b_rat = FAB$M_CR;	/* carriage returns on ends */
	fab.fab$b_rfm = FAB$C_VAR;	/* variable length file */

	rab.rab$l_fab = &fab;
	rab.rab$l_rop = RAB$M_WBH;	/* write behind - multibuffer */

	if (0)
	{
		/* append mode */
		rab.rab$l_rop = RAB$M_EOF;
		status=SYS$OPEN(&fab);
		if (status == RMS$_FNF)
		    status=SYS$CREATE(&fab);
	}
	else	/* *mode == 'w' */
	{
		/* write mode */
   		fab.fab$l_fop |= FAB$M_MXV; /* always make a new version */
		status=SYS$CREATE(&fab);
	}

	if (successful(status))
	{
	     status=SYS$CONNECT(&rab);
	}

        if (unsuccessful(status))
            return -1;
        return 1;
}

/*
 * Close a file. Should look at the status in all systems.
 */
int ffclose()
{
	unsigned long status;

	/* free this since we do not need it anymore */
	if (fline) {
		free(fline);
		fline = NULL;
	}

	status = SYS$DISCONNECT(&rab);
	if (successful(status)) status = SYS$CLOSE(&fab);
	else SYS$CLOSE(&fab);

        if (unsuccessful(status)) {
                return FIOERR;
        }
        return FIOSUC;
}

extern int ucrypt(char *, int);

/*
 * Write a line to the already opened file. The "buf" points to the buffer,
 * and the "nbuf" is its length, less the free newline. Return the status.
 * Check only at the newline.
 */
int ffputline(char buf[], int nbuf)
{
        register char *obuf=buf;

        /* set output buffer */
	rab.rab$l_rbf = obuf;
	rab.rab$w_rsz = nbuf;

	if (unsuccessful(SYS$PUT(&rab))) {
                return(FIOERR);
        }

        return(FIOSUC);
}

/*
 * Read a line from a file, and store the bytes in the supplied buffer. The
 * "nbuf" is the length of the buffer. Complain about long lines and lines
 * at the end of the file that don't have a newline present. Check for I/O
 * errors too. Return status.
 */
int ffgetline(int * nbytes)

{
	unsigned long status;

	/* if we don't have an fline, allocate one */
	if (fline == NULL)
	{ g_flen = fab.fab$w_mrs ? fab.fab$w_mrs+2 : 32769;
	  fline = (char*)malloc(g_flen);
	    
	  if (fline == NULL)
	    return(FIOMEM);
	}

	/* read the line in */
	rab.rab$l_ubf = fline;
	rab.rab$w_usz = g_flen;

	status=SYS$GET(&rab);
	*nbytes = rab.rab$w_rsz + 1;
	if (status == RMS$_EOF) return(FIOEOF);
        if (unsuccessful(status)) {
                return(FIOERR);
        }

	/* terminate and decrypt the string */
        fline[rab.rab$w_rsz] = '\n';
        fline[rab.rab$w_rsz+1] = 0;
        
        return(FIOSUC);
}


int count_file()

{  int ct = 0;
 
   while (1)
   { int cc = ffgetline(&flinelen);
     if (cc != FIOSUC)
        break;
     ct += flinelen; 
   }
   
   return ct;
}


#endif
#endif



int chan_read(FStream fid, char * t, Short ct_)

{ Int actual;
  Short ct = ct_;

#if S_VMS
  while (ct > 0)
  { ++fline_ix;
    while (fline_ix >= flinelen)
    { fline_ix = 0;
    { int cc = ffgetline(&flinelen);
      if (cc != FIOSUC)
      { flinelen = 0;
        return ct_-ct;
      }
    }}
    *t++ = fline[fline_ix];
    --ct;
  }

  return ct_;
#else
  while (ct > 0 and (actual = read(fid, &t[0], ct)) > 0)
  { t += actual;
    ct -= actual;
  }
  return ct_ - ct;
#endif
}


Cc chan_write(int fid, char * t, Short ct)

{ int actual;
#if S_VMS
  while (ct > 0 and (actual = fwrite(&t[0], 1, ct, (FILE*)fid)) > 0)
#else
  while (ct > 0 and (actual = write(fid, &t[0], ct)) > 0)
#endif
  { t += actual;
    ct -= actual;
  }
  
  return ct == 0 ? OK : errno;
}

extern Short      this_drv;			/* from rdflop.c */

/* static */ Cc change_volume(char * st, int rdw)

{ if (this_drv < 0)
  { if (rdw == FLOP_WR)
      fprintf(stderr, "archive is out of space\n");
    return HALTED;
  }

  while (true)
  { Char ch;
#if S_MSDOS && S_WIN32 == 0
    while ( _bios_keybrd(_KEYBRD_READY))
      _bios_keybrd(_KEYBRD_READ);
#else
    Id ip = open("/dev/tty", O_RDONLY);
#if S_WIN32 == 0 && S_WIN32 == 0
    signal(SIGALRM, SIG_IGN);
    alarm(1);
#endif
    while (read(ip, &ch, 1) > 0)
      ;
#endif
    rdx_fini(tl_opt);
    fprintf(stderr, "Change medium and press CR");

#if 1
#if S_MSDOS && S_WIN32 == 0
    ch = _bios_keybrd(_KEYBRD_READ) & 0xff;
    if (ch != A_CR)
      exit(0);
#else
    close(ip);
#endif
#endif
  { Cc cc = rdx_init(c_opt, z_opt, tl_opt, flcyls, devnm);
    if (cc != OK)
      fprintf(stderr, "Device not found\n");
    else
    { cc = rdx_rw(st, c_opt ? FLOP_WR : FLOP_RD);
      if (cc != OK)
        fprintf(stderr, "Cannot %s archive device\n", c_opt ? "write" : "read");
      return cc;
    }
  }}
}

int prot_open(char * targname, int mode)

{ int prot = not k_opt ? 0 : O_EXCL;
#if S_VMS
  int cc;
  if (mode & O_WRONLY)
    cc = (int)fopen(targname, "w");
  else
    cc = open(targname, mode | prot, S_IREAD|S_IWRITE);

  if (cc == NULL)
#else
  int cc = open(targname, mode | prot, S_IREAD|S_IWRITE);
  if (cc < 0)
#endif
  { if (k_opt and errno == EEXIST)
    { fprintf(stderr, "Protected: %s\n", targname);
    } 
  }
  return cc;
}

static Int put_compress(Char * filenm, Int size, Int mtime)

{ tardcr.s.mode= 0774;
  if (idcr_t.linkflag)
   tardcr.s.mode |= M_LINKED;

  tardcr.s.size = switch_big(size);
  tardcr.s.mtime= switch_big(mtime);

{ int olen = strlen(last_name_linkname);
  int nlen = strlen(filenm);
  int len = nlen;
  
  if (len > olen)
    len = olen;
{ int slix = -1;
  int diffix = -1;
  int ix;
  for (ix = -1; ++ix < len; )
  { if (filenm[ix] != last_name_linkname[ix] and diffix < 0)
      diffix = ix;
    if (filenm[ix] == '/')
      slix = ix;
  }
  if	  (diffix <= 2)
  { diffix = 0;
    olen = 0;
  }
  else if (diffix >= olen)
  { tardcr.s.name_linkname[0] = ';';
    olen = 1;
  }
  else if (filenm[diffix-1] == '/' and slix < diffix)
  { tardcr.s.name_linkname[0] = '*';
    olen = 1;
  }
  else
  { sprintf(&tardcr.s.name_linkname[0], "%d;", olen - diffix);
    olen = strlen(tardcr.s.name_linkname);
  }
  strpcpy(&tardcr.s.name_linkname[olen], &filenm[diffix], 
				    sizeof(tardcr.s.name_linkname)-olen);
  strpcpy(&last_name_linkname[0], filenm, sizeof(last_name_linkname));
{ Short offs = fieldoffs(Stardcr, name_linkname) + 
				  	strlen(tardcr.s.name_linkname)+1;
  Short sz = TBLOCK - offs;
  if (sz > 0)
  { if (sz > size)
      sz = size;
    size -= sz;
    this_cc = chan_read(ipfile, &tardcr.b[offs], sz);

    if (this_cc < sz)
    { fprintf(stdout, "Error %d reading file %s\n", this_cc, filenm);
      size = 0;
    }
  }
  tardcr.s.size |= (Int)calc_cs((Byte*)&tardcr) << 24;
  return size;
}}}}

				/* READ THE FILE, WRITE THE ARCHIVE or
				   PASS FILES ACROSS */
static Cc write_filedir(char * filenm)

{ Int got = rdf_writespace();
  Key_t k_t;  
  if (filenm[0] == '.' and
      filenm[1] == '/')
    filenm += 2;
{ Vint fnmlen = strlen(filenm);
  k_t._str = (Char*)malloc(fnmlen+1);
  strcpy(&k_t._str[0], filenm);
{ Char * * eny = dict_add(dict, Q_STR, k_t);
  struct stat fstat_;
  if (was_there)
    return HALTED;
#if 1
  if (name_is_diry(filenm))
  { int trash;
    Short direny_ct = 0;
    Char * fn;

#if S_VMS
    DIR * curr = opendir(filenm);
/*eprintf(null, "%d %d OPen %s\n", msd_ix, msd_chd, dir);*/
    if (curr != NULL)
    { struct DIRENT *eny;                 /* directory entry pointer */
      while ((eny = (struct DIRENT*)readdir(curr)) != NULL)
      { char buf[132];
        char * fn = eny->d_name;
        int len = fnmlen + 1 + strlen(fn);
        if (len < sizeof(buf))
        { strcpy(buf,filenm);
          strcat(buf,"/");
          strcat(buf,fn);
          (void)write_filedir(buf);
        }
      }
      closedir(curr);
    }
#else
    msd_init(filenm, "", MSD_DIRY | MSD_REPEAT | MSD_AUTO |
		     MSD_HIDFILE | MSD_SYSFILE);
    
    while ((fn = msd_nfile(&trash)) != null)
      if (msd_attrs & MSD_DIRY)
        msd_push(MSD_REPEAT);
      else
        (void)write_filedir(fn);
#endif
    return OK;
  }
#endif

  memset(&tardcr, 0, sizeof(tardcr));      

  if (strcmp(filenm, devnm) == 0)
  { fprintf(stdout, "Tar file ignored: %s\n", devnm);
    return OK;
  }
  
  if (filenm[0] == ':')				/* ':' => filler */
    fstat_.st_size = got;
  else
  { 
#if S_VMS
    Cc rc = ffropen(filenm);
    if (rc == FIOSUC)
    { /*fprintf(stdout, "Got %s\n", filenm);*/
      fstat_.st_mtime = 0;
      fstat_.st_size = count_file();
      (void)ffclose();
      rc = ffropen(filenm);
    }
    if (rc != FIOSUC)
#else
    Cc rc = 0;
    ipfile = open(filenm, O_RDONLY+O_MY_BINARY);
    if (ipfile < 0)
#endif
    { if (filenm[fnmlen-1] != '/')
        fprintf(stdout, "F (%d,%d) %s\n", errno,rc, filenm);
      return NOT_FOUND;
    }        

  { 
#if S_VMS == 0
    Cc rc = fstat(ipfile, &fstat_);
    if (rc != OK)
    { fprintf(stdout, "Cannot stat %s\n", filenm);
      return NOT_FOUND;
    }
#endif
  
#if S_MSDOS == 0 && S_WIN32 == 0 && S_VMS == 0
    idcr_t.linkflag = 0;
    k_t._int = (fstat_.st_dev << 16) + fstat_.st_ino;
    eny = dict_add(idict, Q_INT, k_t);
    if (not was_there)
      *eny = filenm;
    else
    { idcr_t.linkflag = 2;
      strpcpy(&tardcr.t.linkname[0], *eny, NAMSIZ+1);
      fprintf(stdout, "File %s linked to %s\n", filenm, *eny);
    }   
#endif
  }}
{ Cc rc = OK; 
#if S_VMS
  Int size = fstat_.st_size;
#else
  Int size = tardcr.t.linkname[0] != 0 and ! S_UNIXTEXT ? 0 : fstat_.st_size;
#endif
/*fprintf(stdout, "Size %d\n", size);*/
  if (s_opt and size < vol_sz and size > got and got != 0)
  { Cc cc = write_filedir(":");
    if (cc == OK)
      cc = write_filedir(filenm);
    fprintf(stderr, "Split inhibited\n\r");
    return cc;
  }
    
  if (passdir != null)
  { Char targname[257];				/* PASS FILES ACROSS */
    Short ix;
    for (ix = -1; ++ix <= 256 and passdir[ix] != 0; )
      targname[ix] = passdir[ix];
    if (passdir[ix-1] != '/' and
        passdir[ix-1] != '\\')
      targname[ix++] = '/';
      
    strpcpy(&targname[ix], skip_dotslash(&filenm[0]), 256 - ix);
  { Cc cc = ensure_path(targname);
    if (cc == OK)
    {
      int op = prot_open(targname, O_WRONLY + O_CREAT + O_MY_BINARY + O_TRUNC);
      if (op < 0)
        cc = NOT_FOUND;
      else
      { while (true)
        {
#if S_VMS
          ix = fread(&trblk[0], 1, tr_blk_sz, ipfile);
#else
          ix = read(ipfile, &trblk[0], TR_BLK_SZ);
#endif
          if (ix <= 0)
            break;
#if S_VMS
          for (ix = -1; ++ix < tr_blk_sz; )
            putc(trblk[ix],(FILE*)op);
#else
          if (write(op, &trblk[0], ix) != ix)
#endif
            fprintf(stderr, "WE");
        }
#if S_VMS
        fclose((FILE*)op);
#else
        close(op);
#endif
      }
    }
    if (cc != OK and errno != EEXIST)
      fprintf(stderr, "Couldn't open %s\n", targname); 
    blk_ct += (fstat_.st_size + 0x400) >> LOG_FS_BLKSZ;
  }}
  else						/* WRITE the ARCHIVE */
  { if (compress_opt)				
      size = put_compress(filenm, size, fstat_.st_mtime);
    else
    { memset(&tardcr.t.name[0], 0, 512); 
      strpcpy(&tardcr.t.name[0], filenm, NAMSIZ);
      strcpy(&tardcr.t.mode[0], "100744 ");
      sprintf(&tardcr.t.uid[0], "%6o ", userno);
      sprintf(&tardcr.t.gid[0], "%6o ", groupno);
      sprintf(&tardcr.t.size[0], "%11lo ", size);
      sprintf(&tardcr.t.size[0], "%11lo ", size);
      sprintf(&tardcr.t.mtime[0], "%11lo ", fstat_.st_mtime);
      memset(&tardcr.t.chksum[0], ' ', 8); 
      sprintf(&block[0], "%6o", calc_cs((Byte*)&tardcr));
      memcpy(&tardcr.t.chksum[0], block, 8);
    }
  { Cc cc = rdx_rw((char*)&tardcr.t, FLOP_WR);
    if (cc != OK)
    { if (cc == HALTED)
        cc = change_volume((char*)&tardcr.t, FLOP_WR);
      if (cc != OK)
        return cc;
    }

    ++blk_ct;
      
    for ( ; size > 0; size -= TBLOCK)
    { int cc;
      Short sz = minv(size, TBLOCK);
      memset(&block[0], 0, TBLOCK+1);
      this_cc = chan_read(ipfile, &block[0], sz);

      cc = rdx_rw(block, FLOP_WR);
      if (cc != OK)
      { if (cc != HALTED)
          fprintf(stdout, "Error archiving file %s\n", tardcr.t.name);
         else
	  cc = change_volume(block, FLOP_WR);
        if (cc != OK)
          return cc;
      }
      blk_ct += 1;
     
      if (this_cc < sz)
      { fprintf(stdout,"Error %d %d reading file %s\n",this_cc,sz,tardcr.t.name);
        /*perror("ReAd");*/
        fstat_.st_size -= sz - this_cc;
   /*   rc = EERROR;
        break; */
      }
    }
  }}
#if S_VMS
  (void)ffclose();
#else
  if (ipfile >= 0)
    close(ipfile);
#endif
  if (v_opt)
    fprintf(stdout, "%c %dK %s, %ld bytes\n", 
             c_opt ? 'c' : 'p', blk_ct >> 1, filenm, fstat_.st_size);
  return rc;
}}}}

static Int get_standard()

{ if (block[0] == 0)
    return -1;

  memcpy(&tardcr.t, block, sizeof(tardcr.t));
  memcpy(&idcr_t.name[0], tardcr.t.name, NAMSIZ);
  sscanf(&tardcr.t.mode[0],  "%8o", &idcr_t.mode);
  sscanf(&tardcr.t.uid[0],   "%8o", &idcr_t.uid);
  sscanf(&tardcr.t.gid[0],   "%8o", &idcr_t.gid);
  sscanf(&tardcr.t.size[0],  "%12lo",&idcr_t.size);
  sscanf(&tardcr.t.mtime[0], "%12lo",&idcr_t.mtime);
  sscanf(&tardcr.t.chksum[0],"%8o", &idcr_t.chksum);

  idcr_t.linkflag = 0;
  if (tardcr.t.linkflag == '1')
  { idcr_t.linkflag = 1;
    strpcpy(&idcr_t.linkname[0], 
             tardcr.t.linkname, sizeof(idcr_t.linkname[0]));
  }
/*      printf( "CS=%x\n", bcs); */
  memset(&tardcr.t.chksum[0], ' ', 8);

{ Int diff = idcr_t.chksum - calc_cs((Byte*)&tardcr);
  if (diff != 0 and diff != 0x220 and diff != 0x38d and diff != 0x232)/* why ? */
    fprintf(stdout, "Is %x sb %x\n", idcr_t.chksum, calc_cs((Byte*)&tardcr));
  return strict_opt and diff != 0 
#if S_VMS
                    and diff != 0x232
#endif
			  ? -2 : idcr_t.size;
}}



static Int get_compressed()

{ memcpy(&tardcr.s, block, TBLOCK);

  idcr_t.mode	= switch_short(tardcr.s.mode);
  idcr_t.uid	= 0;
  idcr_t.gid	= 0;
  idcr_t.size	= switch_big(tardcr.s.size) & 0xffffff;
  idcr_t.mtime  = switch_big(tardcr.s.mtime);
  idcr_t.chksum = ((Byte*)&tardcr.s.size)[3];
  idcr_t.linkflag = idcr_t.mode & M_LINKED;
  idcr_t.mode &= ~M_LINKED;
  ((Byte*)&tardcr.s.size)[3] = 0;

{ Vint ix;
  Vint olen = strlen(last_name_linkname);
  Vint nlen = strlen(tardcr.s.name_linkname);
  if (nlen == 0)
    return -1;
  if ((idcr_t.chksum ^ calc_cs((Byte*)&tardcr)) & 0xff)
    return -2;

  for (ix = nlen; 
       --ix >= 0 and tardcr.s.name_linkname[ix] != ';'
                 and tardcr.s.name_linkname[ix] != '*'; )
    ;
  if (y_opt)
    ix = -1;
  if (ix < 0)
    olen = 0;
  else if (tardcr.s.name_linkname[ix] == '*')
  { for (; --olen > 0 and last_name_linkname[olen] != '/'; )
     ;
    ++olen;
  }
  else
  { int chop = atoi(tardcr.s.name_linkname);
    if (chop > 0)
      olen -= chop;
  }
  strpcpy(&last_name_linkname[olen], &tardcr.s.name_linkname[ix+1], 
			sizeof(last_name_linkname) - 1 - ix );
  strpcpy(&idcr_t.name[0], last_name_linkname, sizeof(idcr_t.name));

  idcr_t.headtext = TBLOCK - 1 - fieldoffs(Stardcr, name_linkname) - nlen;

{ Int sz = idcr_t.size - idcr_t.headtext;
  return sz < 0 ? 0 : sz;
}}}

void main(int argc, char ** argv)

{ if (argc <= 1)
  { explain();
    exit(0);
  }
  
  lend.big = 1;

{ Int csum;

  Vint argsleft;
  Char ** argv_ = &argv[1];

  Int logchan;
  FILE * fip = stdin;

  for (argsleft = argc; --argsleft > 0; )
  { if (argv_[0][0] != '-')
      break;

  { Char * s = &argv_[0][0];
    if (s[1] == 0)
      sio_opt = true;
  
    while (*++s != 0)
    { if      (in_range(*s, '0', '9'))
      { logchan = atol(s);
        while (isdigit(*s))
          ++s;
        --s;
         /* popen */
      }
      else if (*s == 'a')
	a_opt = true;		/* in ascii mode */
      else if (*s == '.')
      { if (system(*++argv_) != 0)
          fprintf(stderr, "System %s", *argv_);
      }
      else if (*s == 'i')
        strict_opt = false;     /* strict checksum mode */
      else if (*s == 't')
        t_opt = true;           /* test dont do */
      else if (*s == 'k')
        k_opt = true;		/* protect existing files */
      else if (*s == 'c')
        c_opt = true;		/* create archive */
      else if (*s == 'r')
        z_opt = true;		/* append to archive */
      else if (*s == 's')
        s_opt = true;		/* dont split files across volumes */
      else if (*s == 'v')
        v_opt = true;		/* verbose */
      else if (*s == 'x')
        x_opt = true;		/* extract */
      else if (*s == 'y')
      { y_opt = true;		/* something to do with links */
        t_opt = true;
      }
      else if (*s == 'b')
        compress_opt = true;
      else if (*s == '-')
        tl_opt = true;		/* ??? */
      else if (*s == '1' and *++s == '4' and *++s == '4')
        flcyls = 18;
      else if (*s == '@')
      { fip = fopen(s+1, "r");
        if (fip == NULL)
        { fprintf(stderr, "Cannot open index file %s\n", s+1);
          exit(0);
        }
        sio_opt = true;
        break;
      }
      else if (*s == '=')
      { fprintf(stderr, "%s\n", VERSION);
        exit(0);
      }
      else
      { --argsleft;
        ++argv_;
	if      (*s == 'u')
	  userno = atoi(*argv_);
	else if (*s == 'g')
	  groupno = atoi(*argv_);
	else if (*s == 'B')
	  vol_sz = atoi(*argv_);
	else if (*s == 'f')
	  devnm = *argv_;
	else if (*s == 'p')
	  passdir = *argv_;
	else if (*s == 'z')
	  termblks = atoi(*argv_);
#if S_VMS
	else if (*s == 'l')
	  tr_blk_sz = atoi(*argv_);
#endif
	else
	{ explain();
	  exit(0);
	}
      }
    }
    
    ++argv_;
  }}

  if (t_opt + x_opt + c_opt + (passdir != null) != 1)
  { fprintf(stdout,"Exactly one of t, c, x, or p please\n");
    explain();
    exit(1);
  }

  dict = dict_new((long)64);
  idict = dict_new((long)64);

  if (passdir != null)
    c_opt = true;
  else
  { if (c_opt and not z_opt)
    { Bool res = name_is_datafile(devnm);
      if (res)
      { fprintf(stderr, "File already exists %s\n", devnm);
        exit(1);
      }
    }
      
  /*fprintf(stderr,"openout:%s\n", devnm);*/
  { Cc cc = rdx_init(c_opt, z_opt, tl_opt, flcyls, devnm);
    if (cc != OK)
    { fprintf(stderr, "Cannot read input %s(%d)\n", devnm, cc);
      exit(1);
    }
  }}

  if (not rdx_has_vols)
    s_opt = false;
  else
   if (vol_sz == 0)
     vol_sz = rdf_diskspace();

  if (x_opt or t_opt)
  { Int size = -1;
    Bool wanted = false;
#if 1
    for (++argsleft; --argsleft > 0; ++argv_)
    { Char * filenm = skip_dotslash(argv_[0]);
      Key_t k_t;  k_t._str = (Char*)malloc(strlen(filenm)+1);
      
      strcpy(&k_t._str[0], filenm);
      (void)dict_add(dict, Q_STR, k_t);
      qualified = true;
    }
#endif

    while (true)
    { int i;
      
      Cc cc = rdx_rw(&block[0], FLOP_RD);
      if (cc == HALTED)
        cc = change_volume(&block[0], FLOP_RD);
      if (cc > OK)
        break;
      if (cc != OK)
      { fprintf(stdout, "Read error on archive (%d)\n", cc);
        exit(1);
      }
      
      if (size <= 0)
      { memset(&idcr_t, 0, sizeof(idcr_t));

        size = not compress_opt ? get_standard() : get_compressed();
        if (size < 0)
        { if (size == -1)
	    break;
	  fprintf(stdout, "Skipping bad CS\n");
	}

	if (idcr_t.name[0] == 0)
	  break;

	if (idcr_t.name[0] == ':')
	{ wanted = false;
	  continue;
	}
      { Char * fname = skip_dotslash(&idcr_t.name[0]);
	  
	wanted = not qualified;

	if (qualified)
	{ Char * t;
	  for (t = &fname[0]; ; ++t)
	    if (*t == '/' or *t == 0)
	    { Char sch = *t;
	      Key_t k_t;  k_t._str = fname;
	      *t = 0;
	      if (dict_read(dict, Q_STR, k_t) != null)
		wanted = true;
	      *t = sch;
	      if (sch == 0)
		break;
	    }
	}
	
	if (not wanted)
	  continue;
	
	line[0] = 0;

  	if (idcr_t.linkflag)
#if S_UNIXTEXT
	  sprintf(&line[0], " was linked to %s", tardcr.t.linkname);
#else
	  sprintf(&line[0], " linked to %s", tardcr.t.linkname);
#endif

	filename_to_dos(idcr_t.name);
	if (t_opt)
	{ if (not v_opt)
	    fprintf(stdout, "%s\n", idcr_t.name);
	  else
	  { static char repn[][4] = {"---", "--x", "-w-", "-wx", "r--", "r-x",
				     "rw-", "rwx"}; 
	    fprintf(stdout, "%s%s%s %3d %3d %8ld	%s%s\n", 
			     repn[(idcr_t.mode >> 6) & 7],
			     repn[(idcr_t.mode >> 3) & 7],
			     repn[(idcr_t.mode) & 7],
			idcr_t.uid, idcr_t.gid, idcr_t.size, idcr_t.name, line);
	  }
	}
	else if (idcr_t.mode & S_IFDIR)
#if S_WIN32
	  mkdir(idcr_t.name);
#else
	  mkdir(idcr_t.name, idcr_t.mode & 0777);
#endif
	else
	{ scan_init();
	  this_cc = ensure_path(idcr_t.name);
	  if (this_cc != OK)
	    fprintf(stderr, "COP %s\n", idcr_t.name); 
	  else
	  { 
#if S_VMS
	    if (this_file != 0)
	      fclose((FILE*)this_file);
#else
	    if (this_file >= 0)
	      close(this_file);
#endif
	    this_file = prot_open(fname, O_WRONLY+O_CREAT+O_MY_BINARY+O_TRUNC);
	    if (this_file < 0)
	    { this_cc = NOT_FOUND;
	      wanted = false;
	    }
	  }
	  if (this_cc != OK)
	  { if (strcmp(failname, idcr_t.name) != 0 and wanted)
	      fprintf(stderr, "Cannot write file %s\n", idcr_t.name);
	    strcpy(&failname[0], idcr_t.name);
	  }
	}
	if (idcr_t.headtext > 0 and x_opt and this_cc == OK and wanted)
	{ Char * blk = &block[TBLOCK - idcr_t.headtext];
	  Short msz = not a_opt ? -1 : scan_block((Byte*)blk, idcr_t.headtext);
	  this_cc = msz < 0 ? chan_write(this_file, block, idcr_t.headtext)
			    : chan_write(this_file, (char*)trblk, msz);
	}
      }}
      else
      { Short sz = minv(size, TBLOCK);
	size -= sz;

	if (x_opt and this_cc == OK and wanted and sz > 0)
	{ Short msz = not a_opt ? -1 : scan_block((Byte*)block, sz);
	  this_cc = msz < 0 ? chan_write(this_file, block, sz)
			    : chan_write(this_file, (char*)trblk, msz);
	}
	
	if (size <= 0 and wanted)
	  if (this_cc != OK)
	    fprintf(stdout, "Error (%d) writing %s\n", this_cc, idcr_t.name);
	  else
	    if (v_opt and not t_opt)
	    { fprintf(stdout, "x %s  %ld bytes\n", tardcr.t.name, idcr_t.size);
	      if (tardcr.t.linkname[0] != 0)
		fprintf(stdout, "%s\n", line);
	    }
      }
    }
  }
  else
  { blk_ct = 0;
#if S_VMS
    ipfile = NULL;
#else
    ipfile = -1;
#endif
  
    for (++argsleft; sio_opt or --argsleft > 0; ++argv_)
    { Char * filenm = sio_opt ? fgets(&line[0], MAXLN, fip)
			      : argv_[0];
      if (filenm == null or *filenm <= ' ')
        break;
      if (sio_opt)
      { int len = strlen(line)-1;
        if (len > 0 && line[len] <= 13)
          --len;
        line[len+1] = 0;
      } 
      (void)write_filedir(filenm);
    }

    if (passdir == null)
      rdx_term(termblks);
  }

  rdf_fini();
  rdx_fini(tl_opt);
  if (x_opt)
  { FILE * log = fopen("tar.log", "a+");
    if (log != NULL)
    { fprintf(log, "File: %s\n", devnm);
    }
  }

  exit(0);
}}
