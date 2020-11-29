#define M_SKIPSP  2

typedef struct Lbuf_s
{ Char * line;
  Vint   avail;
  Vint   len;  
  Byte   props;
} Lbuf_t, *Lbuf;


extern Char null_line[1];

Char * ffgetline(Lbuf lbuf, FILE * fd);

