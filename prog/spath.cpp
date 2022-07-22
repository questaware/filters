#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include "build.h"
#if S_MSDOS == 0
#include <unistd.h>
#endif
#include "../../util/strmpath.h"




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
	int incix = 0;
  Char pathbuf[10000];
  pathbuf[0] = ':';
  pathbuf[1] = 0;
  
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
      else if (*flgs == 'v')
        opts |= M_VOPT;
      else if (*flgs == 'I')
      { char * inc = flgs+1;
      	int sl = strlen(inc);
      	if (sl == 0)
      	{	++argv_;
      		--argsleft;
      		if (argsleft == 0)
      			continue;
      		inc = argv_[0];
      		sl = strlen(inc);
      	}
      	pathbuf[incix] = ':';
      	
      	if (incix+sl < sizeof(pathbuf) - 1)
      	{ strcpy(pathbuf+incix+1, inc);
      		incix += sl + 1;
      	}
      	break;
      }
      else
      { explain();
      }
    ++argv_;
  }

{ Char * filename = argv_[0];
  Char * path = getenv("PATH");
  Char * res;
  int got = 0;

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
    path = pathbuf[1] != 0 ? pathbuf+1 : argv_[1];
    
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

{ std::map<std::string, int> dict;

	char * newpath = (char*)malloc(strlen(path)+1);
	char * stt = path;
	*newpath = 0;
	for (char * s = path - 1; ++s; )
	{ char ch = *s;
		if (ch == 0 || ch == ':')
		{ *s = 0;
			if (dict.find(stt) == dict.end())
			{ dict[stt] = 1;
				strcat(strcat(newpath, stt), ":");
			}
			if (ch == 0)
				break;
			stt = s + 1;
		}
	}

	path = newpath;
//opts |= 0x100000;
}

	if (opts & M_PRT)
	{ printf("File %s Path %s\n", filename, path);
		exit(0);
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

		got = 1;
    sho(res);
    if (u_opt)
      break;
  }

	if ((opts & M_VOPT) && !got)
		sho("Not found");

  return 0;
}}}

