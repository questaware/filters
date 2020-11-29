#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "build.h"
#if S_MSDOS == 0
#include <unistd.h>
#endif
#include "../../util/h/strmpath.h"




void explain(void)

{ fputs("spath [-ax/] filename [path]\n"
 "   -a -- Print all findings\n"
 "   -d -- Print only directory part of names\n"
 "   -p -- Print PATH\n"
 "   -x -- Allow only executable files\n"
 "   -/ -- Print out as absolute filenames\n"
 "   -\\ -- Show / as \\\n"
 "  where path is a colon or space separated list of directories optionally\n"
 "        prefixed by -I\n"
 "  where path defaults to $PATH\n"
 "  where if path is a single word it is taken as a variable\n", stdout);
    exit(0);
}

void sho(Char * txt)

{ fputs(txt, stdout);
  fputs("\n", stdout);
}



int main(Vint argc, Char **argv)

{ if (argc <= 1)
  { explain();
  }
  
{ Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];

  Set opts = 0;
  Bool u_opt = true;
  
  for (; argsleft > 0 and argv_[0][0] == '-'; --argsleft)
  { Char * flgs;
    for (flgs = &argv_[0][1]; *flgs != 0; ++flgs)
      if      (*flgs == '/')
        opts |= M_AOPT;
      else if (*flgs == '\\')
        opts |= M_BSLOPT;
      else if (*flgs == 'a')
        u_opt = false;
      else if (*flgs == 'd')
        opts |= M_DOPT;
      else if (*flgs == 'p')
        opts |= M_PRT;
      else if (*flgs == 'x')
        opts |= M_EOPT;
      else
      { explain();
      }
    ++argv_;
  }

{ Char * filename = argv_[0];
  Char * path = getenv("PATH");
  Char * res;
  Char pathbuf[10000];
  pathbuf[0] = 0;   

  if (opts & M_PRT)
  { printf("Path:%s\n", path);
    exit(0);
  }

  if (argsleft < 1)
  { explain();
  }

  if (argsleft > 1)
  { /*if (path != NULL)
      strcpy(pathbuf, path);*/
    for (; --argsleft > 0; ++argv_)
    { char * apath = argv_[1];
      if (apath[0] == '-' && apath[1] == 'I')
        apath += 2;
      if (pathbuf[0] != 0)
        strcat(pathbuf, ":");
      strcat(pathbuf, apath);
    }
    path = pathbuf;
    if (strchr(path,'/') == NULL && 
        strchr(path,'\\') == NULL)
    { char * npath=getenv(path);
      if (npath == NULL || *npath == 0)
      { fputs("path variable not set:", stdout);
        fputs(path, stdout);
        fputs("\n", stdout);
        explain();
      }
      path = npath;
    }
  }

#if S_MSDOS
  for (res = filename; *res != 0; ++res)
    if (in_range(*res, 'a', 'z'))
      *res += 'A' - 'a';
#endif

  if (filename[0] == '/')
    sho(filename);
  else
  while ((res = srchpath(filename, path, opts)) != null)
  { 
    if (opts & M_BSLOPT)
    { char * t;
      for (t = res; *t != 0; ++t)
        if (*t == '/') *t = '\\';
    }

    sho(res);
    if (u_opt)
      break;
  }

  return 0;
}}}
