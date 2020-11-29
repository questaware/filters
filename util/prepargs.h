#define NARGS 27

extern Int args[NARGS];

#define ARGS(c) args[c-'A']

extern Bool helpwanted;


extern Bool moreargs __((Short, Char **, Char **));

