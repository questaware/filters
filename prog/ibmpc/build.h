#ifndef BUILDINC
#define BUILDINC
			/*  ENVIRONMENT SELECTION SECTION */
#define S_UNIX5 0
#define S_XENIX 0
#define S_MSDOS 1
#define S_WIN32 0
#define S_SOLARIS 0

#define MSC 1
#undef __STDC__
#define NONDIR 0                        /* use ndir.h not dirent.h      */

		/* Count net malloc area */
#define COUNT_MEM 0

#define COLOR 1

#define ANSI	0			/* ANSI escape sequences	*/
#define TERMCAP 1			/* Use TERMCAP			*/
#define	IBMPC	0			/* IBM-PC CGA/MONO/EGA/VGA drvr	*/

#define LONG_ALGNMT 2		/* smallest alignment for longs */
#define SIZEOF_LONG 4           /* size of long */
#define SIZEOF_PTR  4           /* size of int * */

#define LITTLEENDIAN 1
#define STACK_GROWS_UP 0

#define CHARCODE ASCII

				/* size for general strings */
#define NSTRING 256
#define FILENAMESZ 256

typedef int   Vint;       /* set this to short int if it is more efficient */
			  /* the following different in Unix: */
typedef long Int;
typedef unsigned long Set;
typedef unsigned long Set32;

#define REVISION "1.1"

#include "../../h/base.h"
#endif /* VERSION */
