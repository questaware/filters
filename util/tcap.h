typedef struct
{	short	t_mrow; 		 /* max number of rows allowable */
	short	t_nrow; 		/* current number of rows used	*/
	short	t_mcol; 		/* max Number of columns.	*/
	short	t_ncol; 		/* current Number of columns.	*/
	short	t_margin;		/* min margin for extended lines*/
	short	t_scrsiz;		/* size of scroll region "	*/
	int	t_pause;		/* # times thru update to pause */
} Term_t;
 
extern Term_t term;

#define CTRL	0x0100	/* Control flag, or'ed in	    */
#define META	0x0200	/* Meta flag, or'ed in		    */
#define CTLX	0x0400	/* ^X flag, or'ed in		    */
#define SPEC	0x0800	/* special key (function keys)	    */
#define MOUS	0x1000	/* alternative input device (mouse) */
#define	SHFT	0x2000	/* shifted (for function keys)	    */
#define	ALTD	0x4000	/* ALT key...			    */

#define A_BLACK   0
#define A_RED     1
#define A_GREEN   2
#define A_YELLOW  3
#define A_BLUE    4
#define A_MAGENTA 5
#define A_CYAN    6
#define A_WHITE   7

#define A_BOLD   0x100
#define A_USCORE 0x200
#define A_BLINK  0x400
#define A_REV    0x800

/* The form to ttXXXX interface is
        Nibble   3         2        1        0
                         Mode     BG col   FG col
*/

extern int ttgetc(/*f*/);
extern int hold();

#if 0
   Avoid void - PASCAL NEAR conflicts 
extern void tcapopen(/*s*/);
extern void tcapclose();

extern void ttputc(/*c*/);
extern void ttmove(/*row, col*/);
extern void tteeol();
extern void tteeop();
extern void ttrev(/*b*/);
extern void ttfcol();
extern void ttbcol();
extern void ttbeep();

extern void ttscup();
extern void ttscdn();
extern void ttcansc();

#endif
