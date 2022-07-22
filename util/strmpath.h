#define M_EOPT 1	/* require files to have the execute bit set */
#define M_AOPT 2	/* print as absolute filenames */
#define M_DOPT 4	/* print only the directory part of file names */
#define M_PRT  8
#define M_BSLOPT 16
#define M_VOPT   32

#ifdef __cplusplus
extern "C" {
#endif

Char * srchpath __((Char *, Char *, Set));
Char * abspath __((Char *));

#ifdef __cplusplus
}
#endif

