#include <stdio.h>
#include <windows.h>
#include <ctype.h>

// extern int input_timeout (unsigned int, unsigned int);

void explain(void)

{ fputs("toupper -#\n"
 "   -#  seconds to delay between lines\n"
 "toupper\n", stdout);
    exit(0);
}

int main(int argc, char **argv)

{ // if (argc <= 1)
  // { explain();
  // }
  
{ int argsleft = argc - 1;
  char ** argv_ = &argv[1];

	int secs = 0;
	
  for (; argsleft > 0 && argv_[0][0] == '-'; --argsleft)
  { char * flgs;
    for (flgs = &argv_[0][1]; *flgs != 0; ++flgs)
      if      (*flgs >= '0' && *flgs <= '9' )
      { secs = atoi(flgs);
//			printf("Getting %s is %d\n", flgs, secs);
				break;
      }
      else
      	explain();
    ++argv_;
  }

//printf("Secs is %d\n", secs);
{	FILE * ip = argsleft <= 0 ? stdin : fopen(argv_[0], "r");
	if (ip == NULL)
	{ fputs("File not found\n", stdout);
	}
	else
	{	if (secs > 0)
			_sleep(secs*1000);
	{ char buff[10000];
		char * ln;
		while ((ln = fgets(buff, sizeof(buff), ip)) != NULL)
		{ char * t;
			for (t = buff-1; *++t != 0; )
				*t = toupper(*t);
			fputs(ln, stdout);
			if (secs > 0)
				_sleep(secs*1000);
		}
	}}
	return 0;
}}}
