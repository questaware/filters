#include <stdio.h>


char ln[200];


/* change the name of a file */
#define rename(old, new) \
{ int cc = link(old, new); \
  return cc != 0 ? cc : unlink(old); \
}


extern char * getffile(), *getnfile();

main ()

{ char * fn = getffile("*");
  while (fn != 0)
  { fprintf(stderr, "Old file name >%s<\nNew file name? ", fn);
    ln[0] = 0;
    fgets(&ln[0], 200, stdin);
    if (ln[0] != 0)
    { ln[strlen(ln)-1] = 0;
      rename(fn, ln);
    }
    fn = getnfile();
  }
}
