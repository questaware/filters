extern Char * strmatch __(( Char *,  Char *));
extern Char * cmdmatch __((Char *,  Char *));
extern Short struccmp __(( const Char *,  const char *));
extern Char * skipspaces __(( Char *));
extern Char * skipalnum __(( Char *));
#ifndef C_STRUTIL
extern Char * strpcpy __(( Char *,  Char *, Short));
extern Char * strpcat __(( Char *,  Char *, Short));
#endif
extern Char * strchcpy __(( Char *, Char, Short));
extern Char * strwcpy __(( Char *, Char *));
extern Char * getalnum __(( Char *, Char *, Short));
extern Char * mem_to_prtable __((Char *,  Char *, Short));

extern Char * pat_strsrchset __((Byte *, Char *, Char *));
extern void pat_dfltsrchset __((Byte *, Char *));
extern void pat_undfltsrchset __((Byte *, Char *));


#define chr_in_set(ch, set) (set[ch])
