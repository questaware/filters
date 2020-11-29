#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include "version.h"
#include "../h/dict.h"


Char dbg_file_name[] = "tlog";

static Int randomm = 654654564;

#define PRIME (2 * 3 * 4 * 5 * 6 * 7 - 1)

static Int gen_randm()

{ Int r = randomm * PRIME;
  randomm = (r >> 4) ^ r;
  r = randomm >> 4;
  return r;
}



Char tdict[0x1000];



main ()

{ Dict dict = dict_new(64);

  Int i;
  access("/etc/motd", 077);
  for (i = 100; --i >= 0; )
  { Key_t key; key._int = gen_randm() & 0xfff;
    fprintf(stdout, "+++ %d\n", key._int);
  { Int * p = (Int*)dict_add(dict, Q_INT, key);
    *p = key._int * 3;
    tdict[key._int] = 1;
  }}

{ Dictstrm ds = dict_new_strm(dict);

  while (true)
  { Char ** eny = dict_next(ds);
    if (eny == null)
      break;
    fprintf(stdout, "--- %d\n", (Int)*eny);
  }

  dict_fini_strm(ds);

  randomm = 654654564;

  for (i = 100; --i >= 0; )
  { Key_t key; key._int = gen_randm() & 0xfff;
    if (tdict[key._int])
    { Int * p = (Int*)dict_read(dict, Q_INT, key);
      if (*p != key._int * 3)
        i_log(0, "Wrong");
#if 1
    { Int v = (Int)dict_del(dict, Q_INT, key);
      if (not was_there or v != key._int * 3)
        i_log(1, "Wrong del");
      tdict[key._int] = 0;
      (void)dict_del(dict, Q_INT, key);
      if (was_there)
        i_log(1, "Wrong del 2");
    }
#endif
    }
    fprintf(stdout, "I is %d\n", i);
  }
 
}}
