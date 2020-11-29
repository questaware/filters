#include   <stdio.h>
#include   <ctype.h>
#include   <fcntl.h>
#include   "build.h"



#define LNSZ 100

#define CMDOFFS 40

char lbuff[LNSZ+1];



void main(argc, argv)
        Short   argc;
        Char ** argv;
{ Vint lno = 0;
  Char * pat = "#line 1";
  
  if (argc > 1)
  { pat = argv[1];
  }
  

  for (lno = 0; ; ++lno )
  { Char * ln = fgets(&lbuff[0], LNSZ, stdin);
    if (ln == null)
      break;
    
  { Vint len = strlen(pat);
    lbuff[len] = 0;
    if (lno > 0 and strcmp(lbuff, pat) == 0)
    { ln = &lbuff[len+1];
      ln[strlen(ln)-2] = 0;
      printf("%s\n", ln);
    }
  }}
}
