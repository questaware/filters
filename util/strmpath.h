#define M_EOPT 1	/* require files to have the execute bit set */
#define M_AOPT 2	/* print as absolute filenames */
#define M_DOPT 4	/* print only the directory part of file names */

extern Char * srchpath __((Char *, Char *, Set));
extern Char * abspath __((Char *));

