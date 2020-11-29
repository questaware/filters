/****************************************************************************/
/* This module supports mappings from a key to a record containing it.	    */
/* The implementation always maintains a sorted array physically contiguous,*/
/* so that insertion is as expensive as block moves and memory reallocations*/
/****************************************************************************/
#include <stdlib.h>
#include "build.h"


#include "map.h"

/*  Note on program conventions: Any type Xxx is Xxx_t * 	*/


#define LENINC 4096




#if USE_MAPSTREAM
#define update_map(map)  (map)->update_ct += 1
#else
#define update_map(map) 
#endif



#ifdef MINIMAP

#define DO_SERIAL 0
#else
#define DO_SERIAL 1

/* 
	Map   		c  	-- location for the result or null
	Format_t	format	-- Format of the reord
	Vint		len	-- length in bytes of the array of records
*/
				     // len in bytes
Map mk_map(Map c, Char type, Short key_offs,  Short eny_len, Vint len)

{
  if (FOFFS_MAP != fieldoffs(Map, c))
    return null;

  if (c == null)
    c = (Map)malloc(FOFFS_MAP+1+len);
  
  if (c != null)
  { c->key_offs = key_offs;
    c->key_type = type;
    c->eny_len = eny_len;
    c->max_len  = len;
  
    c->curr_len = 0;
    c->curr_mult = 0;
    c->last_ix = -1;
    c->c[0] = 0;
  }
  return c;
}


#endif


static Vint compare(Map map, Key tgt)

{ Char * key = map->srch_key;

  switch (map->key_type)
  { when T_DOMSTR:  /* printf("Comparing %s %s\n", map->srch_key, tgt->domstr);*/
                      return  strcmp(key, tgt->domstr);
#ifndef MINIMAP
    when T_DOMCHAR0:  return  1;  /* never matches */
    case T_DOMINT1:   return  (signed Char)key - tgt->domint1;
    when T_DOMINT2:   return  (Short)key - tgt->domint2;
    when T_DOMINT3:   return  0; /* not implemented */
    when T_DOMINT4:   return  (Int)key - tgt->domint4;
#endif
    otherwise       { Vint ix;
                      key -= T_DOMCHAR0;
		   /* printf("Compare %s %s\n", map->srch_key, tgt->domchararr);*/
  		      for (ix = T_DOMCHAR0-1;  ++ix < map->key_type; )
		      { Char tch = tgt->domchararr[ix-T_DOMCHAR0];
			if (key[ix] != tch)
                          return (Byte)key[ix] - (Byte)tch;
      		        if (tch == 0)
      		          break;
      		      }
  		      return OK;
		    }
  }
}


/* 
	Map   		map  	-- The map to search, but map.c is not valid!
	Byte * 		table	-- this replaces map.c
*/

Vint bin_unit_len;		/* secondary result */

				/* +ve => found */
				/* -ve => before index (minus one based!) */
Vint binary(Map map, Byte * table)

{ register Vint low = 0;		/* search limit is entire list */
  register Vint upp = map->curr_mult-1;
{ Byte * keyfld = &table[map->key_offs];
#if DO_SERIAL
  if (map->key_type == T_DOMCHAR0)		/* not binary */
  { bin_unit_len = 1;
    if (upp < 0)
      return -1;
    while (true)
    { Byte * src = (Byte*)map->srch_key;
      Vint comp = 0;
      Vint slow = low;

      for (; ; ++low)
      { if (comp == 0)
          comp = *src - (Vint)keyfld[low];
	if (keyfld[low] == 0)
	  break;
	++src;
      }
      low += map->key_offs + 1;
      map->eny_len = low - slow;

      if (comp >= 0 or low > map->curr_len)
        return comp == 0 ? slow : -(slow+1);
    }
  }
  else
#endif
  { bin_unit_len = map->eny_len;
    while (upp >= low)
    { Vint i = (low + upp) >> 1;			/* get the midpoint! */

      Vint comp = compare(map, (Key)&keyfld[map->eny_len*i]);
      if (comp == 0)
        return i;
      if (comp < 0)
        upp = i - 1;
      else
        low = i + 1;
    }

    return -(upp+2); /* one before->after; one to change base */
  }
}}


#ifndef MINIMAP

	    /* need be called only if */
	    /* key points at the same location, but *key changes */
void map_cache_clear(Map map)	

{ map->last_ix = -1;
}



/* 
	Map  *  	map_ref -- INOUT
	Byte * 		rec	-- the record to add
*/
Cc map_add_(Map * map_ref, Byte * rec)

{ Map map = *map_ref;
  Char * key = &rec[map->key_offs];
  Byte kt = map->key_type;
  map->srch_key = kt  >  T_DOMSTR ?  key                :
  		  kt == T_DOMINT1 ? (Char*)key[0]	: /* warning OK */
  		  kt == T_DOMINT2 ? (Char*)*(Short*)key : /* warning OK */
  		  kt == T_DOMINT3 ? 0            	:
  		  		    *(Char**)key;
{ Vint ix = - binary(map, map->c) * bin_unit_len;
  if (ix <= OK)
  { map->last_ix = -ix;
    return HALTED;
  }

  ix -= bin_unit_len;
  map->last_ix = ix;
{ Vint inc;
  Vint len = kt != T_DOMCHAR0 		/* length of record */
                        ? map->eny_len 
                        : map->key_offs + strlen(&rec[map->key_offs]) + 1;
  
  if (len + map->curr_len > map->max_len)
  { /* Vint */ inc = len;
    if (inc < LENINC)
      inc = LENINC;
    inc += map->curr_len;			/* length of map */
    map = (Map)malloc(FOFFS_MAP+1+inc);
    if (map == null)
      return ENOSPACE;
    map->max_len = inc;
    map->is_mallocd = true;
    memcpy(map, *map_ref, fieldoffs(Map, c) + ix);
  }

  inc = map->curr_len - ix + 1;			/* to move */
  if (inc > 0)
    memmove(&map->c[ix+len], &(*map_ref)->c[ix], inc);

  map->curr_len += len;
  map->curr_mult += 1;
  update_map(map);
  memcpy(&map->c[ix], rec, len);
  if (map != *map_ref and (*map_ref)->is_mallocd)
    free(*map_ref);
  *map_ref = map;
  return OK;
}}}



/* 
	Map    		map	-- the map to search but map->c is not valid.
	Byte * 		table	-- replaces map->c
	void *		key 	-- the key to search for
*/
Byte * map_find_(Map map, Byte * table, void * key)

{ Vint ix = map->last_ix;
  Byte * eny = &table[ix];

  map->srch_key = key;
  if (key == null and map->key_type >= T_DOMSTR)
    map->srch_key = "";
  
  if (not in_range(ix,0, map->curr_len-1) or 
      compare(map, (Key)&eny[map->key_offs]) != OK)
  { ix = binary(map, table) * bin_unit_len;
    if (ix < OK)
    { map->last_ix = -(ix +  bin_unit_len);
      return null;
    }
    map->last_ix = ix;
    eny = &table[ix];
  }
  return eny;
}



/* 
	Map *   	map	-- INOUT, map->c is not valid
	Byte * 		table	-- replaces map->c
*/
void map_remove_last_(Map * map_ref, Byte * table) /* remove last record found*/

{ Map map = *map_ref;
  memmove(&table[map->last_ix],
          &table[map->last_ix+map->eny_len], 
          map->curr_len-map->last_ix);
  map->curr_len -= map->eny_len;
  map->curr_mult -= 1;
  update_map(map);
}

#endif

#if USE_MAPSTREAM


/* 
	Mapstrm *   	strm 	-- location for the result or null
	Map		map     -- The map to stream through, map->c not valid
	void * 		key	-- Where to strart the streaming
*/
		/* the key must be maintained constant if of type T_DOMSTR */
Mapstrm mk_strm_(Mapstrm strm, Map map, void * key) /* make a stream */

{ Vint bigsz = map->key_type - T_DOMCHAR0;
  if (strm == null)
    strm = (Mapstrm)malloc(sizeof(Mapstrm_t) + (bigsz < 0 ? 0 : bigsz));

  strm->map        = map;
  strm->update_ct  = -1;
  strm->key.domstr = key;
  if (bigsz > 0)
    memcpy(&strm->key.domchararr[0], key, bigsz);
  return strm;
}

/* This streaming function reseeks when others add or remove */

/* 
	Mapstrm *   	strm 	-- INOUT The stream
	Byte * 		table	-- The actual sorted array, replaces map->c
	void * 		key	-- Where to strart the streaming
*/
Byte * map_next_(Mapstrm strm, Byte * table)	/* return the next record */

{ Map map = strm->map;
  Vint bigsz = map->key_type - T_DOMCHAR0;

  map->srch_key = map->key_type < T_DOMCHAR0
                      ? strm->key.domstr : &strm->key.domchararr[0];

  if (strm->update_ct == map->update_ct
      /* and not (compare(map, (Key)&eny[map->key_offs]) != OK and
                  strm->ix < map->curr_len) */)
    map->last_ix += map->eny_len;
  else
  { if (map_find_(map, table, bigsz <= 0 ? strm->key.domstr
			                : &strm->key.domchararr[0]) != null and
        strm->update_ct != -1)			/* first time */
      map->last_ix += map->eny_len;
      
    strm->update_ct = map->update_ct;
  }

{ Byte * eny = &table[map->last_ix];
  if      (bigsz < 0)
  { bigsz = bigsz == T_DOMSTR - T_DOMCHAR0 ? sizeof(char*) : bigsz + T_DOMCHAR0;
  }
  else if (bigsz == 0)
  { bigsz = strlen(&eny[map->key_offs]) + 1;  /* requires a 0 at EO map */
    map->eny_len = map->key_offs + bigsz;
  }
  if (map->last_ix >= map->curr_len)
  { map->last_ix -= map->eny_len;
    return null;
  }

  memcpy(&strm->key.domchararr[0], &eny[map->key_offs], bigsz);
  return eny;
}}

#endif

#if 1

typedef struct
{ int  a;
  char key[6];
  int  b;
} Eny_t, *Eny;


main()

{ Eny_t enys[] = {{1, "Fred", 2}, 
                  {3, "Ann", 4},
                  {5, "Mary", 6},
                  {7, "Jane", 8},
                  {9, "Bob", 10}};
  Map map = mk_map(null, 
                   T_DOMCHAR0+6, fieldoffs(Eny, key), sizeof(enys[0]), 20);
  
  Vint ix;
  for (ix = upper_index(enys)+1; --ix >= 0; )
  { Cc cc = map_add(&map, &enys[ix]);
    if (cc != OK)
      printf("Couldnt add %d\n", ix);
  }

  for (ix = upper_index(enys)+1; --ix >= 0; )
  { Eny rec = (Eny)map_find(map, &enys[ix].key[0]);
    if (rec == null)
      printf("Couldnt find %d\n", ix);
    else
      printf("Found %s for %s\n", rec->key, enys[ix].key); 
         /* problem with string termination !! */
  }

  for (ix = upper_index(enys)+1; --ix >= 0; )
  { Eny rec = (Eny)map_find(map, &enys[ix].key[0]);
    if (rec == null)
      printf("Couldnt find %d\n", ix);
    else
    { printf("Deleting %s for %s\n", rec->key, enys[ix].key); 
         /* problem with string termination !! */
      map_remove_last(&map);
    }
  }
}

#endif
