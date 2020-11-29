#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include "version.h"
#include "../h/dict.h"

Char dbg_file_name[] = "tlog";

Char line[400];

Set dict_mode = Q_INT;

main ()

{ Dict dict = dict_new(64);
  Int i;
  
  if (fgets(&line[0], 390, stdin) == null)
    exit(0);
    
  if (line[0] == 'i' or line[1] == 'i')
    dict_mode |= Q_INT;
  if (line[0] == 's' or line[1] == 's')
  { dict_mode &= ~Q_INT;
    dict_mode |= Q_STR;
  }
  if (line[0] == 'm' or line[1] == 'm')
    dict_mode |= Q_MULT;
  
  while (fgets(&line[0], 390, stdin) != null)
  { Key_t key; key._str = null;
  { Int sl = strlen(line);
    sl -= 1;
    line[sl] = 0;
    if (sl > 2)
      if (dict_mode & Q_INT)
        key._int = atol(&line[2]);
      else
      { key._str = malloc(sl);
        strcpy(&key._str[0], &line[2]);
      }
    switch (line[0])
    { when 'a': 
      { Char * * p = dict_add(dict, dict_mode, key);
        if (was_there)
          printf("Was There\n");
        else
          *p = key._str;
      }
      when 'd':
      { Char * p = dict_del(dict, dict_mode, key);
        if (was_there)
          printf(dict_mode & Q_INT ? "Was %d\n" : "Was %s\n", p);
      }
      when 'D':
      { Bool ok = dict_del_last(dict);
        if (ok)
          printf("Deleted\n");
      }
      when 'r':
      case 'R':
      { Char * * eny = line[0] == 'R' ? dict_read_next(dict, dict_mode)
      				      : dict_read(dict, dict_mode, key);
	if (eny == null)
	  printf("Not there\n");
	else
          printf(dict_mode & Q_INT ? "Is %d %x\n" : "Is %s %x\n", *eny, eny);
      }
      when '?':
      { Dictstrm ds = dict_new_strm(dict);

	while (true)
	{ Char ** eny = dict_next(ds);
	  if (eny == null)
	    break;
	  fprintf(stdout, dict_mode & Q_INT ? "--- %d\n" : 
					      "--- %s\n", (Int)*eny);
	}

	dict_fini_strm(ds);
      }
    }
  }}
}
