#ifndef IN_ESPRINTF
#if __STDC__ || 1
 int eprintf(Char * tgt, const Char * __format, ...);
#else
 int eprintf();
#endif


extern Int spare_sprintf_arg;


extern void fputsout(const char * sheny);
extern void fputserr(const char * sheny);
extern void msg_str(const char * msg, const char * str);


#endif
