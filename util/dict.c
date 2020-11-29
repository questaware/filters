#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include "build.h"
#include "dict.h"
/*#include "h/malloc.h"*/

extern Char * malloc();

#define chk_malloc malloc
#define chk_free(x) free((char*)x)

#define DFLT_SZ 64

#define KEYOFFS sizeof(Dicteny)

static Dicteny_t freeslot;		/* must contain nulls */

static Dicteny last_cell;		/* Location of last cell. */ 
					/* It is written by dict_add,dict_read*/
static Dicteny last_prev;		/* Location of last prev cell. */
					/* It is written by dict_add,dict_read*/
					/* It is null if last_cell is */
					/* in the hash table.	      */

Dict dict_new(Int sz_)

{ Short cursor = 1;
  while (cursor < sz_)
    cursor *= 2;
{ Short sz = cursor == 1 ? DFLT_SZ : cursor;
  Dicteny de = (Dicteny)chk_malloc(sizeof(Dicteny_t) * sz);
  Dict dict = (Dict)chk_malloc(sizeof(Dict_t));
  dict->sz  = sz;
  dict->c   = de;
  
  memset(&de[0], 0, sizeof(Dicteny_t) * sz);
  while (--sz >= 0)
    de[sz].next = &freeslot;
  return dict;
}}



static Int hash(Quot typ, Key_t key, Int sz)

{
  if (typ & Q_INT)
    return (key._int * 13) & (sz - 1);
  else
  { fast Int res;
         Char * str = key._str;
    res = str[0] * 17;
    
    while (*++str != 0)
      res = res * 13 + str[0];
      
    return res & (sz - 1);
  }
}

Bool was_there;

Char * * dict_add(Dict dict, Set typ, Key_t key)

{ was_there = false;
{ Dicteny de = &dict->c[hash(typ, key, dict->sz)];
  if (de->next == &freeslot)
    de->next = null;
  else
  { Dicteny eny = de;
    last_prev = null;

    for (; eny != null; eny = eny->next)
    { Int sign = typ & Q_STR ? strcmp(eny->key._str, key._str)
			     : eny->key._int - key._int;
      if (sign >= 0)
      { if (sign > 0)
	  break;
	was_there = true;
        if (typ & Q_MULT)
          break;
	return &eny->c;
      }
      last_prev = eny;
    }
    
  { Dicteny neny = (Dicteny)chk_malloc(sizeof(Dicteny_t));
    if (last_prev == null)
    { memcpy(&neny[0], de, sizeof(Dicteny_t));
      de->next = neny;
    }
    else
    { last_prev->next = neny;
      neny->next = eny;
      de = neny; 
    }
  }}
  last_cell = de;
  last_cell->key = key;
  return &de->c;
}}



Char * * dict_read(Dict dict, Set typ, Key_t key)

{ Dicteny de = &dict->c[hash(typ, key, dict->sz)];
  if (de->next != &freeslot)
  { last_prev = null;

    for (; de != null; de = de->next)
    { Int sign = typ & Q_STR ? strcmp(de->key._str, key._str)
	        	     :        de->key._int - key._int;
      if (sign >= 0)
      { if (sign > 0)
          break;
        last_cell = de;
        return &de->c;
      }
      
      last_prev = de;
    }
  }
  
  last_cell = null;
  return null;
}



Char * * dict_read_next(Dict dict, Set typ)

{ if ((typ & Q_MULT) == 0)
    last_cell = null;
  if (last_cell == null)
    return null;

{ Dicteny de = last_cell;
  Char * key = last_cell->key._str;
  
  if (de != null)
  { last_prev = de;
    de = de->next; 
    if (de != null)
      if (typ & Q_INT ? (Int)key == de->key._int : strcmp(key, de->key._str) == 0)
      { last_cell = de;
        return &de->c;
      }
  }

  last_cell = null;
  return null;
}}



void reloc_last_cell(Key_t key)

{ if (last_cell != null)
    last_cell->key = key;
}



static Char * dict_bdel(Dicteny de, Dicteny prev)

{ Char * res = de->c;
  Dicteny freeeny = null;

  if (de->next == null)
    if (prev == null)
      de->next = &freeslot;
    else
    { freeeny = de;
      prev->next = freeeny->next;
    }
  else
  { freeeny = de->next;
    memcpy(&de[0], freeeny, sizeof(Dicteny_t));
  }
  if (freeeny != null)
    chk_free(freeeny);
  was_there = true;
  return res;
}



Char * dict_del(Dict dict, Set typ, Key_t key)

{ was_there = false;
  last_cell = null;
{ Dicteny de = &dict->c[hash(typ, key, dict->sz)];
  if (de->next != &freeslot)
  { Dicteny de_ = de;
    Dicteny prev = null;
  
    for (; de != null; de = de->next)
    { Int sign = typ & Q_STR ? strcmp(de->key._str, key._str)
	        	     :        de->key._int - key._int;
      if (sign >= 0)
      { if      (sign > 0)
          break;
	return dict_bdel(de, prev);
      }
      prev = de;
    }
  }
  
  return null;
}}



Bool dict_del_last(Dict dict)

{ was_there = false;
  if (last_cell == null)
    return false;
  was_there = true;
{ Dicteny de = last_cell;
  (void)dict_bdel(de, last_prev);
  last_cell = null;
  return true;
}}

Dictstrm dict_new_strm(Dict dict)

{ Dictstrm ds   = (Dictstrm)chk_malloc(sizeof(Dictstrm_t));
  ds->dict      = dict;
  ds->ix        = -1;
  ds->curr_list = &freeslot;
  return ds;
}



void dict_fini_strm(Dictstrm strm)

{ free(strm);
}




Char * * dict_next(Dictstrm strm)

{ Dict dict = strm->dict;
  Dicteny cl = strm->curr_list->next;
  
  if (cl != null)
  { strm->curr_list = cl;
    return &strm->curr_list->c;
  }
  else
    while (true)
    { strm->ix += 1;
      if (strm->ix >= strm->dict->sz)
        return null;
      strm->curr_list = &strm->dict->c[strm->ix];
      if (strm->curr_list->next != &freeslot)
        return &strm->curr_list->c;
    }
}
