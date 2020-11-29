#define _ALL_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#if S_MSDOS
#include <io.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include "build.h"
#include "../common/h/msdir.h"
#include "../common/h/eprintf.h"
#include "../util/h/getline.h"

extern char * malloc();


#ifndef S_IREAD
# define S_IREAD S_IRUSR
#endif


char * invokenm;

char buf[128];
char opbuf[300];

typedef struct Inocell_s * Inocell;

typedef struct Inocell_s
{ Inocell  next;
  Int      ino;
} Inocell_t, *Inocell;


Inocell_t inohd;



local void explain()

{ 
  eprintf(null, "srename        (prompts for new names for files in .\n");
}

void main(argc, argv)
        int     argc;
        char ** argv;
{      char * eny;
       int trash;
  fast Vint ix;
  if (argc > 1)
    explain();
  invokenm = argv[0];

  msd_init("./", "", MSD_DIRY | MSD_REPEAT | (MSD_HIDFILE+MSD_SYSFILE));
    
  while ((eny = msd_nfile(&trash)) != null)
  {
#if 0
    Inocell cell = (Inocell)malloc(sizeof(Inocell_t));
    Inocell ptr;
    for (ptr = &inohd; 
        (ptr = ptr->next) != null and ptr->ino != msd_stat.st_ino;)
      ;
      
    if (ptr != null)
      continue;
    
    if (cell != null)
    { cell->next = inohd.next;
      inohd.next = cell;
    } 
#endif
    eny += 2;
    eprintf(null, "%d >%s<  to be called, (CR no change):\n",
            msd_stat.st_ino, eny);
  { char * ln = fgets(&buf[0], sizeof(buf), stdin);
    int len = strlen(buf) - 1;
    if (len > 0 and ln != null)
    { buf[len] = 0;
      if (buf[len-1] == '/')
        buf[len-1] = 0;
      len = strlen(eny);
      if (eny[len-1] == '/')
        eny[len-1] = 0;
#if S_MSDOS
      eprintf(&opbuf[0], "move %s %s", eny, ln);
#else
      eprintf(&opbuf[0], "mv %s %s", eny, ln);
#endif
      system(opbuf);
    }
  }}
}
