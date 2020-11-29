#define FLOP_RESET (0)
#define FLOP_RD (2)
#define FLOP_WR (3)
#define FLOP_READ_CMOS (0x08)
#define FLOP_SEEK  (0x0c)
#define FLOP_READY (0x10)
#define FLOP_RECAL (0x11)


void rdf_fini();
Cc rdf_read(Char * t);
Cc rdf_write(Char * t);
Cc rdx_init(Bool rw, Bool app, Bool tl_opt, Vint cyls, Char * s);
void rdx_term(Vint termblks);
void rdx_fini(Bool tl_opt);
Cc rdx_rw(Char * t, Short wh);
Cc rdf_writespace();
Cc rdf_diskspace();

