#ident "@(#) dllcvt.h 1.1 progs 1/11/93 dllcvt.h";
/******************************************************************************
*  
* TITLE NAME: dll.h    
*  
* PURPOSE: This module is the dll header file.
*          
*  
* CALLING SEQUENCE: 
*                  Input Parms   - 
*                                 
* 
*                  Output Parms  -
*                                 
* MODIFICATION HISTORY:
* No.     Date      By    Version   Description
* ---  ----------   ---   -------   -------------------------------------------
* 00   Sep 12  89   twm   1.1.1.a   Orignal
* 
******************************************************************************/
unsigned char atoh [80] = {
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  /* . . . . . . . .  00-07 */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  /* . . . . . . . .  08-0f */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  /* . . . . . . . .  10-17 */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  /* . . . . . . . .  18-0f */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  /* . . . . . . . .  20-27 */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  /* . . . . . . . .  28-2f */
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,  /* 0 1 2 3 4 5 6 7  30-37 */
0x08, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  /* 8 9 . . . . . .  38-3f */
0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff,  /* . A B C D E F .  40-47 */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  /* . . . . . . . .  48-4f */
};

FILE *ifp, *ofp ,*savfp, *tmpfp;

unsigned long int i_rec_cnt = 0;    /* number of I data records processed */
unsigned long int file_cnt = 0;     /* byte count of entire file */
unsigned long int block_cnt = 0;    /* byte count of a block */

struct tm *newtime;
long ltime;

unsigned int option = 0;
unsigned char o1;
unsigned char o2;
unsigned char flush;

int crc_table[256];
unsigned char crc_char;
unsigned short crc;

int i;

int count;

unsigned char describ, x1, x2;

unsigned long int c  = 0;           /* ASCII char input */

unsigned long int t1 = 0;           /* nibble 1 of record type */
unsigned long int t2 = 0;           /* nibble 2 of record type */
unsigned long int rec_type = 0;     /* record type 0 = data, 1 = end */

unsigned long int c1 = 0;           /* nibble 1 of checksum */
unsigned long int c2 = 0;           /* nibble 2 of checksum */
unsigned long int checksum = 0;     /* check sum of record*/
unsigned long int cksum = 0;        /* running check sum of record*/

unsigned long int b1 = 0;           /* nibble 1 of byte count */
unsigned long int b2 = 0;           /* nibble 2 of byte count */
unsigned long int byte_count = 0;   /* byte count of record*/

unsigned long int a1 = 0;           /* nibble 1 of address */
unsigned long int a2 = 0;           /* nibble 2 of address */
unsigned long int a3 = 0;           /* nibble 3 of address */
unsigned long int a4 = 0;           /* nibble 4 of address */
unsigned long int address = 0;      /* address read */
unsigned long int nxt_address = 0;  /* incrementing address */
unsigned long int last_address = 0; /* save last address */

unsigned long int d1 = 0;           /* nibble 1 of data */
unsigned long int d2 = 0;           /* nibble 2 of data */
unsigned long int data = 0;         /* data */

