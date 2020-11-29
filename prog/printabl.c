#include <stdio.h>
#include <fcntl.h>

#include "build.h"



explain()

{ printf("printabl [<file>]\n");
  exit(1);
}

#if S_MSDOS == 0

#define O_BINARY 0

#endif


main(int argc, char * argv[])

{ int ip = argv[1] == NULL ? 0 : open(argv[1], O_RDONLY+O_BINARY);
  if (ip < 0)
    explain();
 
{ int ct;
  char ch;
  while ((ct = read(ip, &ch, 1)) > 0)
  { if (ch >= ' ' && ch <= 'z' || ch == '\t' || ch == '\n' || ch == '\r')
     putc(ch, stdout); 
  }
}}
