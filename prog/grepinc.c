#include   <stdio.h>
#include   <ctype.h>
#include   <fcntl.h>
#include   "build.h"

#include "../common/h/eprintf.h"


extern char * malloc();

#if S_MSDOS
#define CLCMD "\\msvc\\bin\\cl /nologo -E"
#else
#define CLCMD "gcc -E"
#endif

#define LNSZ 100

#define CMDOFFS 40

char lbuff[LNSZ+1];



Char * invokenm;


private void explain()

{ fprintf(stderr, "grepinc {-c cmd } {-I diry}* fname\n"
		  "   -c cmd      -- Use cmd for cc\n"
		  "   -p pat      -- pattern to indicate line one of a file\n"
		  "   -n          -- Say what you would do\n"
		  "   -Dvar{=val} -- Define preprocessor variable\n"
		  "   -I diry     -- Include diry in include path\n");
}




void main(argc, argv)
        Short   argc;
        Char ** argv;
{ Char * l1pat = "1 ";
  Bool dont_do = false;
  Bool same = true;
  if (argc <= 1)
  { explain();
    exit(0);
  }
  invokenm = argv[0];

{ Char * ipath = malloc(CMDOFFS+3);
  Char * dpath = malloc(2);
  Char * cmd = CLCMD;
  Vint argsleft;
  Char ** argv_ = &argv[1];

  ipath[CMDOFFS] = 0;
  dpath[0] = 0;

  for (argsleft = argc + 1; --argsleft > 0; )
  { if (argv_[0][0] != '-')
      break;

  { Char * s = &argv_[0][0];
  
    while (*++s != 0)
    { if      (*s == 'c')
      { cmd = *++argv_;
        --argsleft;
      }
      else if (*s == 'n')
        dont_do = true;
      else if (*s == 'p')
      { l1pat = *++argv_;
        --argsleft;
      }
      else if (*s == 'I' or *s == 'D')
      { if (*s == 'I')
        { Vint esz = strlen(&ipath[CMDOFFS]);
          Char * iele = *++argv_;
          Char * nip = malloc(esz+strlen(iele)+4+CMDOFFS);
          if (nip != null)
          { eprintf(&nip[CMDOFFS], "%s -I %s ", &ipath[CMDOFFS], iele);
            free(ipath);
            ipath = nip;
          }
          --argsleft;
        }
        else
        { Vint esz = strlen(dpath);
          Char * nip = malloc(esz+strlen(s)+4);
          if (nip != null)
          { eprintf(&nip[0], "%s -D%s ", dpath, &s[1]);
            free(dpath);
            dpath = nip;
          }
          s = "";
        }
      }
      else
      { explain();
        exit(0);
      }
    }
    
    ++argv_;
  }}

  freopen("nul", "w", stderr);

  for (; --argsleft > 0; )
  { Char * fn = *argv_++;
    eprintf(&ipath[0], l1pat[0] == '"' ? "%s %s %s %s | ginc %s" : "%s %s %s %s | ginc \"%s\"",
    			cmd, &ipath[CMDOFFS], dpath, fn, l1pat);
    if (dont_do)
      printf("%s\n", ipath);
    else
    {
#if S_MSDOS
      shellprog(ipath);
#else
      system(ipath);
#endif
    }
  }
}}
