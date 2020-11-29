#include <stdio.h>


main()

{ char *       * const pc = NULL;
  char * const * const pv = pc;

  printf("%x\n", pv);
}



p1()

{ const char *  * pc = NULL;
        char * * const pv = pc;

  printf("%x\n", pv);
}



p1()

{ const char * * pc = NULL;
        char * * pv = pc;

  printf("%x\n", pv);
}

