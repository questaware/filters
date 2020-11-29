
#include <stdio.h>
#include <ctype.h>

#include <string>
#include <vector>
#include <map>

using namespace std;



void explain()

{ printf("cleangraph { file } \n");
  exit(1);
}


char buf[133];


FILE * ip;


void cleansect(int indent)

{
  map<string, int> dict;
  
  do
  { int ix;
    for (ix = -1; buf[++ix] == ' '; )
      ;
    if (ix > indent) 
    { cleansect(indent+3);
      for (ix = -1; buf[++ix] == ' '; )
        ;
    }
    if (ix < indent) break;

    if (buf[ix] < ' ')
      printf("\n");
    else
    { map<string, int>::iterator dcr = dict.find(buf);

      if (dcr == dict.end())
      { printf("%s", buf);
        dict[buf] = 1;
      }
    }
  }
  while (fgets(buf, sizeof(buf)-1, ip) != NULL);
}



int main (int argc, char * argv[])

{ 
  char * ln;
  int rec_ct = 0;

  char * fn = NULL;

  int argsleft = argc;
  char ** argv_ = &argv[1];

  for (; --argsleft > 0; ++argv_)
  { if (argv_[0][0] == '-')
    { explain();
      --argsleft;
      ++argv_;
    }
    else
    { if (fn != NULL)
         explain();
      fn = argv_[0];
    }
  }

  ip  = fn == NULL ? stdin : fopen(fn, "r");
  if (ip == NULL)
  { printf("Cannot open file %s\n", fn);
    explain();
  }

  ln = fgets(buf, sizeof(buf)-1, ip);
  if (ln != NULL)
    cleansect(0);

  return 0;
}
