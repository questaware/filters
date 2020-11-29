#include <stdio.h>

void sghdlr(n)

{ FILE * err = fopen("/tmp/pjssg", "a+");
  fprintf(err, "Signal %d caught\n", n);
  fclose(err);
  signal(n, sghdlr);  
}


main()

{ int i; /* setbuf(stdout, 0); */
/*for (i = 1; i < 19; ++i)
    signal(i, sghdlr);
*/
  fprintf(stderr, "subproc: PGiD %d\n", getpgrp());

  while (1)
  { /* printf("."); */
    sleep(5);
  }
}
