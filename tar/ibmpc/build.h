#ifndef BUILD_H
#define BUILD_H
			/*  ENVIRONMENT SELECTION SECTION */
#define S_UNIX5 0
#define S_XENIX 0
#define MSDOS   1
#define S_MSDOS 1
#define S_WIN32 0

#define MSC 1
#define __STDC__ 1
#define NONDIR 0                        /* use ndir.h not dirent.h      */

#define USE_UBAR 0

		/* Count net malloc area */
#define COUNT_MEM 0

#define COLOR 1

#define ANSI	0			/* ANSI escape sequences	*/
#define TERMCAP 1			/* Use TERMCAP			*/
#define	IBMPC	0			/* IBM-PC CGA/MONO/EGA/VGA drvr	*/

#define SCRN_COLS 80			/* not used for TERMCAP */
#define SCRN_LINES 25

#define LONG_ALGNMT 2		/* smallest alignment for longs */
#define SIZEOF_LONG 4           /* size of long */
#define SIZEOF_PTR  4           /* size of int * */

#define LITTLEENDIAN 1

#define CHARCODE ASCII

#define MAXSINT 0x7fff
#define MINSINT 0x8000
				/* size for general strings */
#define NSTRING 256

typedef int   Vint;       /* set this to short int if it is more efficient */

			/*  CONDITIONAL FEATURES SECTION */

#if CHARCODE == ASCII

#define A_BEL   7
#define A_BS    8
#define A_HT    9
#define A_LF    10
#define A_FF    12
#define A_CR	13
#define	A_SP	32

#endif

#if SIZEOF_LONG == 4
# define MAXINT 0x7fffffff
# define MININT 0x80000000
#else
# define MAXINT 0x7fff
# define MININT 0x8000
#endif
#define NaN MININT

#if __STDC__
#define __(x) x
#define null (void *)0
#else 
#define __(x) ()
#define null 0L
#endif

#define CANCHOWN  (S_UNIX5)

#if	S_ATARI || S_MSDOS || S_OS2
#define	DIRSEPSTR	"\\"
#define	DIRSEPCHAR	'\\'
#else
#define DIRSEPSTR	"/"
#define	DIRSEPCHAR	'/'
#endif

#if	S_ATARI || S_MSDOS
# define FILENAMESZ 80
#else
# define FILENAMESZ 255
#endif

#if USE_UBAR
#define REGS _REGS
#define SREGS _SREGS
#endif

#if COUNT_MEM
#define free(x)     chk_free __((char*)x)
#else
#define free(x)     free((char*)x)
#endif

#define new(typ)  (typ *)malloc(sizeof(typ))

                                      /* INTERNAL FEATURES SECTION */
			/* levels: 0, 1, 2 */
#define DEBUG 1

#define WYSIWYG_DELAY 2 /* seconds */

#include "../../h/base.h"
#endif /* VERSION */
