			/* all these enumeration values cannot change */
#define T_DOMINT1     1
#define T_DOMINT2     2
#define T_DOMINT3     3
#define T_DOMINT4     4
#define T_DOMSTR      5
#define T_DOMCHAR0    8  /* variable length *//* key must be at end */
#define T_DOMCHAR1    9
#define T_DOMCHAR2   10



#if SIZEOF_PTR <= 4
#define FOFFS_MAP 22
#else
#define FOFFS_MAP 26
#endif


typedef struct Map_s
{ Byte     key_type;
  Byte	   is_mallocd;
  Short    key_offs;
  Short    eny_len;	/* Fixed: the lngth of entry, else: length found */
  Char *   srch_key;	/* These 2 form a pair.   See Note 1. */
  Short    last_ix;

  Short    max_len;	/* in bytes */
  Short    curr_len;	/* in bytes */
  Short    curr_mult;   /* in entries */
  Int	   update_ct;
			  /* must be long aligned for 64 bit machines */
  Byte     c[1];    /* actually Mapeny_t *//* must be aligned to SIZEOF_PTR */
} Map_t, *Map;




typedef union Key
{ Char   domint1;
  Short  domint2;
  Int    domint4;
  Char * domstr;
  Char   domchararr[4];
} Key_t, *Key;



/*		Note 1. The user must not modify the location pointed 
			when streaming for type T_DOMSTR
*/
typedef struct Mapstrm_s
{ Map	    map;
  Int       update_ct;
  Key_t     key;		/* the key last seen */
} Mapstrm_t, * Mapstrm;




#define map_alloc(reclen, len)\
  (Map)malloc(FOFFS_MAP + (reclen) * (len));



#define mk_const_map(typ,keyoffs, tbl) \
		     {{typ, keyoffs, sizeof(tbl[0])}, \
		       null, -1, sizeof(tbl), sizeof(tbl), \
		       sizeof(tbl)/ sizeof(tbl[0]), 0, {0}, \
                     }


Map mk_map(Map c, Char type, Short key_offs, Vint len);
void map_cache_clear(Map map)	;

	/* the xxx_const() functions separate Map and the sorted array */
	/* the xxx() functions bundle them together */

				/* these should NOT be use by the client */
Vint binary(Map map, Byte * table);
Cc map_add_(Map * map_ref, Byte * rec);
Byte * map_find_(Map map, Byte * table, void * key);
void map_remove_last_(Map * map_ref, Byte * table);
Mapstrm mk_strm_(Mapstrm strm, Map map, void * key);
Byte * map_next_(Mapstrm strm, Byte * table);

				/* these should be used by the client */
#define map_add(map, rec) map_add_((map), (Byte*)(rec))
#define map_find(map, key) map_find_((map), &(map)->c[0], (void*)(key))
#define map_remove_last(map) map_remove_last_(map, &(*map)->c[0])
#define map_strm(strm, map, key) map_strm_(strm, map, (void*)key)
#define map_next(strm)    map_next_((strm), &((strm)->map->c)[0])


#define binary_const(map,tbl)       binary(map,(Byte*)(tbl))
#define map_find_const(map,tbl,key) map_find_((map), (Byte*)(tbl), (void*)(key))
#define map_next_const(strm,tbl)    map_next_((strm), (Byte*)(tbl))


/* WARNING: The key parameter to map_find() and map_find_const() is
            he key and not a pointer to the key, except of course for
            T_DOMSTR, and T_DOMCHARn, as usual.
   WARNING: The last parameter to mk_map is the length in bytes of map->c
*/
