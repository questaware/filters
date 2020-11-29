#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "build.h"



main(argc, argv)
    int argc;
    char ** argv;
{ if (argc <= 1)
  {  fprintf(stderr, "unlink filename\n");  
     exit(0);
  }

  unlink(argv[1]);
}

