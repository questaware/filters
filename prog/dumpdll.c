/******************************************************************************
* TITLE NAME: dumpdll.c
*  
* PURPOSE: This program prints dll files.
*  
* CALLING SEQUENCE: 
*                  Input Parms   - input_file
*                                 
* MODIFICATION HISTORY:
* No.     Date      By    Version   Description
* ---  ----------   ---   -------   -------------------------------------------
* 00   Jul 16 93    PJS   1.1.1.a   Orignal
******************************************************************************/
#include "time.h"
#include "stdio.h"
#include "math.h"
#include "base.h"



int crc_table[256];
unsigned short crc;		    /* output crc */
unsigned short act_crc;		    /* crc found in the file */


#define nxt_crc(crc, ch) (crc_table[crc >> 8 ^ ch] ^ crc << 8)


private void explain()

{ fprintf(stderr, "dumpdll ipfile\n");
}

void main(argc, argv)
        Int     argc;
        Char ** argv;
{ Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];
  
  Char * ipfilename = null;

  if (argsleft <= 0)
  { explain();
    exit(0);
  }

  for (; argsleft > 0; --argsleft)
  { if (argv_[0][0] == '-')
    { 
      { explain();
        exit(0);
      }
    }
    else
    { Char * fn = argv_[0];
      if      (ipfilename == null)
        ipfilename = fn;
      else
      { explain();
        exit(0);
      }
    }
    ++argv_;
  }

						/** open input file **/
{ FILE * ifp = fopen(ipfilename, "r");
  if (ifp == NULL) 
  { fprintf (stderr,"\nCannot open input file %s\n", ipfilename);
    exit(1);
  }
{ Short count, i; 
						/** build CRC table **/
  for (count = 0; count < 256; count++)
  { unsigned short crc = count << 8;

    for (i = 8; --i >= 0; )
      if (crc & 0x8000)
        crc = ( crc << 1 ) ^ 0x1021;
      else 
        crc <<= 1;

    crc_table[count] = crc;
/*  printf("%5x", crc_table[count]); */
  }
						       /** read the ip file **/
{ Int ct = 32;
  Int c;
  
  while (--ct >= 0 and (c = getc(ifp)) != EOF)
    printf("%c", c);

  printf("\n");

  ct = 24;
  while (--ct >= 0 and (c = getc(ifp)) != EOF)
    printf("%c", c);
  
  c = getc(ifp);
  printf("%02x", c);
  c = getc(ifp);
  printf("%02x", c);
  
  crc = 0;
  c = getc(ifp);
  crc = nxt_crc(crc, c);
{ Int blk_sz = 0;
  Int flen = c * 256;

  c = getc(ifp);
  crc = nxt_crc(crc, c);
  flen += (c & 0xff);
  
  printf("\nFile len: %d (hex: %x)\n", flen, flen);

  flen -= 0x38;					/* take away the header */
      
  while (true)
  { Int c1 = getc(ifp);
    Int c2 = getc(ifp);
    Int count = c1 * 256 + c2;
    
    if (c1 == EOF or c2 == EOF)
    { printf("Premature EOF\n");
      break;
    }
    
    crc = nxt_crc(crc, c1);
    crc = nxt_crc(crc, c2);
    flen -= 2;
  
  { Int a1 = getc(ifp);
    Int a2 = getc(ifp);
    Int address = a1 * 256 + a2;
  
    if (a1 == EOF or a2 == EOF)
    { printf("Premature EOF\n");
      break;
    }
  
    crc = nxt_crc(crc, a1);
    crc = nxt_crc(crc, a2);
    flen -= 2;
  
    blk_sz = count == 0xffff8000 ? 0 :
	     count == 0          ? 0 :
	     count  > 0          ? count - 2 : address;

    if      (count == 0xffff8000)
      printf("New Offset %x", address);
    else if (count == 0)
    { Int hacrc = getc(ifp);
      act_crc = hacrc * 256 + getc(ifp);
      flen -= 2;
    
      printf("End of File (CRC = %x)\n", act_crc & 0xffff);
      break;
    }
    else if (count > 0)
    { printf("Count %d  Address %x\n", count, address & 0xffff);
    }
    else
    { printf("Repeat Count %d  Of %d\n", count, address & 0xffff);
    }

  { Int pct = 0;
  
    while (--blk_sz >= 0)
    { Int c = getc(ifp);
      if (c == EOF)
      { printf("Premature EOF\n");
        break;
      }
      flen -= 1;
      crc = nxt_crc(crc, c);
      printf(" %02x", c & 0xff);
      if ((++pct & 0xf) == 0)
        printf("\n");
    }
    
    printf("\n");
    if (blk_sz >= 0)
      break;
  }}}

  printf("Act crc %x Calculated crc %x\n", act_crc, -crc & 0xffff);
  printf("File size discrepancy %d\n", flen);
}}}}}  
  
