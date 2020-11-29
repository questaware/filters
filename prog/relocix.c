#include   <stdio.h>
#include   <ctype.h>
#include   <fcntl.h>
#include   "build.h"

#include "../common/h/eprintf.h"



#define LNLEN 256


static Char filename[FILENAMESZ];
static Char cwdstr[FILENAMESZ];
static Char line[LNLEN];
static Char nline[LNLEN];


Char dbg_file_name[] = "relocixdbg";   /* to resolve the symbol */


private void explain()

{ fprintf(stderr, "relocix { -a } { file }*\n");
  fprintf(stderr, "   -a -- print as absolute names\n");
  fprintf(stderr, "   -u -- dont relocate names\n");
}

private Char * to_abs(t, s)
	 Char *t, *s;
{ if (s[0] == DIRSEPCHAR)
    return strcpy(&t[0], s);
  
{ Char tmp[FILENAMESZ]; strpcpy(&tmp[0], s, FILENAMESZ);

  strpcpy(&t[0], cwdstr, FILENAMESZ);
{ Char * te = &t[strlen(t)];
  *te++ = DIRSEPCHAR;
  *te = 0;
  return strcat(&t[0], tmp);
}}}



private Char * simplify(t, s)
	 Char * t, * s;
{ fast Char * tt = t;

  while (*s != 0)
  { if      (s[0] == '.' and s[1] == '.' and s[2]  == DIRSEPCHAR)
    { --tt;
      while (--tt >= t and *tt != DIRSEPCHAR)
        ;
      if (tt < t or *tt == DIRSEPCHAR)
      { tt += 1;
        s += 3;
      }

      continue;
    }
    else if (s[0] == '.' and s[1] == DIRSEPCHAR)
    { s += 2;
      continue;
    }
    
    while (*s != DIRSEPCHAR and *s != 0)
      *tt++ = *s++;
      
    if (*s == DIRSEPCHAR)
      *tt++ = *s++;
  }
  
  *tt = 0;
  return t;  
}

void main(argc, argv)
        Int     argc;
        Char ** argv;
{ if (argc <= 1)
  { explain();
    exit(0);
  }
  
  getcwd(&cwdstr[0], FILENAMESZ);

{ Int argsleft = argc - 1;
  Char ** argv_ = &argv[0];
  
  Bool unreloc = false;
  Bool abspath = false;
  
  for (; argsleft > 0; --argsleft)
  { ++argv_;
    if   (argv_[0][0] == '-')
    { if (argv_[0][1] == 'a')
      { abspath = true;
        continue;
      }
      if (argv_[0][1] == 'u')
      { unreloc = true;
        continue;
      }
      explain();
      exit(0);
    }

  { Char * fn = strcpy(&filename[0], argv_[0]);
    
    if (abspath)
      fn = to_abs(&fn[0], fn);
    fn = simplify(&fn[0], fn);
                 
  { Char * fnend = &fn[strlen(fn)];
    while (--fnend > fn and *fnend != DIRSEPCHAR)
      ;
    
    *fnend = 0;
    
  { FILE * fd = fopen(argv_[0], "r");
    if (fd == null)
      fprintf(stderr, "Cannot open file %s\n", argv_[0]);
    else 
    { while (fgets(&line[0], LNLEN, fd) != null)
      { Char * ln = &line[0];
        while (*ln != 0 and isspace(*ln))
          ++ln;
      
        if      (ln[0] == DIRSEPCHAR)
          fputs(ln, stdout);
        else if (ln[0] != 0)
        { eprintf(&nline[0], "%s/%s", filename, ln);
          simplify(&line[0], nline);
          fputs(line, stdout);
        }
      }
      fclose(fd);
    }
  }}}}
}}
