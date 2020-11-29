#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "build.h"
#include "../common/h/msdir.h"
#include "../common/h/eprintf.h"

extern char * malloc();


#define CRYPT 1

#define FALSE 0
#define TRUE 1


Bool encr = 0;
Bool decr = 0;
Bool listout = 0;


Char * cryptkey = null;



private void explain()

{ puts("docrypt { -k | +k | -l } key {dir|filename}*\n");
}


Bool is_diry(fd)

{	struct stat fstat_;
	int cc = fstat(fd, &fstat_);
	return cc == OK and (fstat_.st_mode & 040000) != 0;
}

#define FIOSUC  0
#define FIOEOF  2
#define FIOMEM (-3)
#define FIOERR (-4)

int eofflag;

FILE * iffp;
FILE * offp;
char * fline = null;
unsigned int flen = 0;

/* Read a line from a file, and store the bytes in the supplied buffer. The
 * "nbuf" is the length of the buffer. Complain about long lines and lines
 * at the end of the file that don't have a newline present. Check for I/O
 * errors too. Return status.
 */
Cc ffgetline(len_ref)
	int	 *len_ref;
{	register int c; 	/* current character read */
	register int i = -1; 	/* current index into fline */

	if (eofflag)			/* if we are at the end...return it */
	  return FIOEOF;
					/* dump fline if it ended up too big */
        if (flen > NSTRING)
	  flen = 0;
							/* read the line in */
	*len_ref = 0;
	do
	{ c = fgetc(iffp);		    /* if it's longer, get more room */
	  if (++i >= flen)
	  { if (flen >= 0xfe00)
	      return FIOMEM;
	  { char * tmpline = malloc(flen+NSTRING+1);
	    if (tmpline == NULL)
	      return FIOMEM;
	    if (fline != null)
	    { memcpy(tmpline, fline, flen);
	      free(fline);
	    }
	    fline = tmpline;
	    flen += NSTRING;
	  }}
	  fline[i] = c;
	} while (c != EOF && c != '\n');

	*len_ref = i;
	fline[i] = 0;
				/* test for any errors that may have occured */
	if (c == EOF)
	{ if (i == 0)
	    return FIOEOF;
	  eofflag = TRUE;
	}
	return FIOSUC;
}

/* Write a line to the already opened file. The "buf" points to the buffer,
 * and the "nbuf" is its length, less the free newline. Return the status.
 * Check only at the newline.
 */
Cc ffputline(buf, nbuf, encrypt)
	char	buf[];
{
	register int i = -1;

#if	CRYPT
	if (encrypt)
	{ while (++i < nbuf)
	  { char c[1]; 	/* character to translate */
	    c[0] = buf[i];
	    crypt(&c[0], 1);
	    fputc(c[0], offp);
	  }
	} 
	else
#endif
	  while (++i < nbuf)
	    putc(buf[i], offp);

	putc('\n', offp);

	if (ferror(offp))
	{ fputs("Write I/O error", stderr);
	  return FIOERR;
	}

	return FIOSUC;
}

void main(argc, argv)
        Vint    argc;
        Char ** argv;
{ if (argc <= 1)
    explain();
  else
{ Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];
  
  for (; argsleft > 0; --argsleft)
  { if (argv_[0][0] == '-' or argv_[0][0] == '+')
    { if (argv_[0][1] == 'k' or
          argv_[0][1] == 'l')
      { if      (argv_[0][0] == '-')
          decr = true;
        if (*(Short*)&argv_[0][0] == *(Short*)"+k")
          encr = true;
       
        if (argv_[0][1] == 'l')
          listout = true;

        ++argv_;
        --argsleft;
        cryptkey = argv_[0];      /* reset encryption key of current buffer */
      }
      else
      { explain();
        argsleft = 0;
      }
    }
    else
    { int trash;
      Char * worklst = null;
      Bool isd = is_diry(argv_[0]);
      Char * fn = argv_[0];
      
      if (isd)
        msd_init(argv_[0], "", MSD_DIRY | MSD_REPEAT | MSD_AUTO | MSD_NOCHD);
    
      while (not isd or (fn = msd_nfile(&trash)) != null)
      { Char * cell = malloc(strlen(fn)+5);
	if (cell != null)
	{ *(Char **)cell = worklst;
	  worklst = cell;
	  strcpy(&cell[sizeof(char *)], fn);
	}
	if (not isd)
	  break;
      }
      
      for (; worklst != null; worklst = *(Char**)worklst)
      { Char opfname[    FILENAMESZ+3];
        Char ipline [2 * FILENAMESZ+12];
        Char * fn = &worklst[sizeof(char*)];
        FILE * chan = fopen(fn, "r");
        Char * fn_ = &fn[strlen(fn)];
        while (--fn_ > fn and *fn_ != '/')
          --fn_;
        strcpy(&opfname[0], fn);
	
	eprintf(stderr, "Doing %s\n", fn);

        strcpy(&opfname[fn_ - fn], "1q7w6s");
        if (chan == null)
          eprintf(stderr, "Cannot open %s\n", fn);
        else
        { FILE * op = fopen(opfname, "w");
          if      (op == null)
            eprintf(stderr, "Cannot open temp file");
          else 
          { if (cryptkey != null) 
             setekey(cryptkey);

            if (true)  /* (not is_diry(chan)) */
            { iffp = chan;
              offp = op;
           
              while (true)
  	      { int len;
  	        int s = ffgetline(&len);
  	        if (s != FIOSUC)
  	          break;
#if	CRYPT
					/* terminate and decrypt the string */
		if (decr)
	          crypt(fline, strlen(fline));
#endif
  	        if (listout)
  	          eprintf(null, "%s\n", fline);
  	        else
  	        { s = ffputline(fline, len, encr);
  	          if (s != FIOSUC)
  	            break;
  	        }
	      }
            }
            fclose(op);
	    if (not listout)
  	    { eprintf(&ipline[0], "del %s", fn);
	      system(ipline);
	      eprintf(&ipline[0], "ren %s %s", opfname, fn);
	      system(ipline);
	    }
          }
          fclose(chan);
        }
      }
    }
    ++argv_;
  }

}}
