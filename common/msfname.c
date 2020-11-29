#define _INCLUDE_POSIX_SOURCE 1
#include        <stdio.h>
#include        <ctype.h>
#include        <string.h>
#include        <fcntl.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include        "build.h"
#if S_MSDOS
# include        <dos.h>
#endif

#include        "h/msfname.h"


#define MYUMASK 0755


#if S_MSDOS
 #define unix_rename(a,b) rename(a,b)
#else

Cc unix_rename(old, new)	/* change the name of a file */
	char *new;	/* new file name */
	char *old;	/* original file name */
{ int cc = link(old, new);
  return cc != 0 ? cc : unlink(old);
}

#endif



Cc replace_file(Char * tfn, Char * sfn)

{	struct stat fstat_;
#if S_MSDOS == 0
        int cc = stat(tfn , &fstat_);
#endif
	int ccc = unlink(tfn);
	if (ccc != OK or unix_rename(sfn, tfn) != OK)
	  return EERROR;
	if (not S_MSDOS && fstat_.st_mode != MYUMASK)
	  chmod(tfn, fstat_.st_mode & 07777);
	return OK;
}
