#ifndef BUILDINC
#define BUILDINC
			/*  ENVIRONMENT SELECTION SECTION */
#define S_UNIX5 1
#define S_BSD   0
#define S_XENIX 0
#define S_MSDOS 0
#define S_WIN32 0
#define S_HPUX  0
#define S_LINUX 1
#if S_LINUX || S_HPUX
 #undef S_UNIX5
 #define S_UNIX5 1
#endif 


#define S_SOLARIS 0
#define S_BORLAND 0


#define MSC 0
#define NONDIR 0                        /* use ndir.h not dirent.h      */

		/* Count net malloc area */
#define COUNT_MEM 0

#define COLOR 1

#define ANSI	0			/* ANSI escape sequences	*/
#define TERMCAP 1			/* Use TERMCAP			*/
#define	IBMPC	0			/* IBM-PC CGA/MONO/EGA/VGA drvr	*/

#define S_64B_PTR   1
#define LONG_ALGNMT 4		/* smallest alignment for longs */
#define SIZEOF_LONG 4           /* size of long */
#define SIZEOF_PTR  4           /* size of int * */

#define LITTLEENDIAN 0

#define STACK_GROWS_UP 0

#define CHARCODE ASCII

				/* size for general strings */
#define NSTRING 256
#define FILENAMESZ 256

typedef int   Vint;       /* set this to short int if it is more efficient */

#pragma pointer_size short

#define REVISION "1.1"


#include <../../h/base.h>


#undef null
#define null 0L

#endif /* VERSION */

