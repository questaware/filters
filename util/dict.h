#define Q_MULT 1
#define Q_STR  2
#define Q_INT  4


typedef union Key_s
{ Int      _int;
  Char *   _str;
} Key_t, * Key;


typedef struct Dicteny_s
{ struct Dicteny_s*  next;
  Key_t		     key;
  Char *	     c;
} Dicteny_t, * Dicteny;

typedef struct Dict_s
{ Int       sz;
  Dicteny   c;
} Dict_t, * Dict;

typedef struct Dictstrm_s
{ Dict    dict;
  Int     ix;
  Dicteny curr_list;
} Dictstrm_t, * Dictstrm;

extern Dict dict_new __((Int));
extern Char * * dict_add __((Dict, Quot, Key_t));
extern Bool was_there;
extern Char * * dict_read __((Dict, Quot, Key_t));
extern Char * dict_del __((Dict, Quot, Key_t));
extern Dictstrm dict_new_strm __((Dict));
extern void dict_fini_strm __((Dictstrm));
extern Char * * dict_next __((Dictstrm));
