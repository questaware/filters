 
#include <stdio.h>
#include "build.h"


Char buff[200];


void explain()

{ fprintf(stderr, "sumup {-x}\n");
  exit(0);
}




void main(argc, argv)
    Vint    argc;
    Char ** argv;
{ double sum = 0;
  Int sumhex = 0;
  Int ct = 0;
  Bool h_opt = false;
  
  if (argc > 1)
    if (argv[1][0] == '-' and argv[1][1] == 'x')
      h_opt = true;
    else
      explain();

  while (true)
  { Char * ln = fgets(&buff[0], 200, stdin);
    if (ln == null)
      break;
    
  { double val;
    Int valhex;
    int c = h_opt ? sscanf(ln, "%lx", &valhex)
                  : sscanf(ln, "%lg", &val);
    if (c > 0)
    { if (h_opt) 
        sumhex += valhex;
      else
        sum += val;
      ct += 1;
    }
  }}
  
  if (h_opt)
    printf("Decimal No of Entries %ld, Hex Sum %lx\n", ct, sumhex);
  else
    printf("No of Entries %ld, Sum %lf\n", ct, sum); 
}
