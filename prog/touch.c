#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "build.h"
#include "../common/h/msdir.h"

extern char * malloc();



Char cmd_line[256]; /* for msera.obj */


private void explain()

{ fprintf(stderr, "touch filename*\n");
  exit(0);
}

/*
Bool is_diry(fd)

{ struct stat fstat_;
  int cc = fstat(fd, &fstat_);
  return cc == OK and (fstat_.st_mode & _S_IFDIR) != 0;
}
*/


Bool touch(fn)
       Char *  fn;
{ Vint fd = open(fn, O_RDWR+O_CREAT);
  if (fd < 0)
    return false;
{ Char ch;
  if (read(fd, &ch, 1) > 0)
  { lseek(fd, 0L,0L);
    write(fd, &ch, 1);
  }
  close(fd);
  return true;
}}

void main(argc, argv)
	Vint	argc;
	Char ** argv;
{ if (argc <= 1)
    explain();
  
{ Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];
  
  for (; argsleft > 0; --argsleft)
  { if	    (argv_[0][0] == '-' or argv_[0][0] == '+')
      explain();
    
    if (strchr(argv_[0],'*') == null)
    { if (not touch(argv_[0]))
      { Vint fd = open(argv_[0], O_CREAT);
	if (fd >= 0)
	  close(fd);
      }
    }
    else
    { int trash;
      Char * worklst = null;
      Char * fn;
      msd_init(argv_[0], null, MSD_DIRY | MSD_REPEAT | MSD_AUTO | MSD_NOCHD);
    
      while ((fn = msd_nfile(&trash)) != null)
      { Char * cell = malloc(strlen(fn)+5);
	if (cell != null)
	{ *(Char **)cell = worklst;
	  worklst = cell;
	  strcpy(&cell[sizeof(char *)], fn);
	}
      }

      for (; worklst != null; worklst = *(Char**)worklst)
	touch(&worklst[sizeof(char*)]);
    }
    ++argv_;
  }
}}
