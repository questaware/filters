#include <stdio.h>
#include "build.h"
#include "../common/h/msdir.h"


#define LSZ 512

Char lbuf[2][LSZ+1];


Bool u_opt = false;
Bool d_opt = false;
Bool c_opt = false;



private void explain()

{ fprintf(stderr,
"uniq -udc filename*\n"
"   -u -- lines not repeated\n"
"   -d -- lines repeated\n"
"   -c -- precede with count\n");
  exit(0);
}

private void doit(f)
      FILE * f;
{ Short mult = 0;
  Vint ct;
  lbuf[1][0] = 0;
  
  for (ct = 0; ; ++ct)
  { Char * ln = fgets(&lbuf[ct & 1][0], LSZ, f);

    if (ln != null and strcmp(ln, lbuf[~ct & 1]) == 0)
      mult += 1;
    else
    { if (not (u_opt | d_opt) or (mult == 0 ? u_opt : d_opt))
      { if (c_opt and mult > 0)
	  printf("%d ", mult+1);
	printf("%s", lbuf[~ct & 1]);
      }
      mult = 0;
    }
    if (ln == null)
      break;
  }
}

void main(argc, argv)
	Short	argc;
	Char ** argv;
{ int trash;
  Short argsleft = argc - 1;
  Char ** argv_ = &argv[1];
  Bool gotfn = false;
  Bool gotstdin = false;

  for (; argsleft > 0; --argsleft)
  { Char * s = &argv_[0][0];
    if (*s == '-')
    { if (*s == 0)
        gotstdin = true;

      while (*++s != 0)
	if      (*s=='u')
	  u_opt = true;
	else if (*s=='d')
	  d_opt = true;
	else if (*s=='c')
	  c_opt = true;
	else
	  explain();
    }
    else
    { Char * eny;
      if (gotstdin)
        doit(stdin);

      msd_init(s, null, MSD_REPEAT | MSD_NOCHD);

      while ((eny = msd_nfile(&trash)) != null)
      { FILE * f = fopen(eny, "r");
	if (f == null)
	  fprintf(stderr, "Cannot open %s\n", eny);
	else
	{ doit(f);
	  fclose(f);
	}
      }
      gotfn = true;
    }
    ++argv_;
  }

  if (not gotfn)
    doit(stdin);
  exit(0);
}
