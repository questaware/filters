#include        <stdio.h>
#include        <fcntl.h>
#include 	<sys/types.h>
#include 	<sys/stat.h>
#include        <errno.h>
#include        "build.h"
#if S_MSDOS == 0
  cannot do
#else
#include        <dos.h>
#include        <io.h>



extern Int atol();


 union REGS rg;		/* cpu register for use of DOS calls */
 struct SREGS segreg;	/* cpu segment registers	     */


#define FDD_FAC (0x13)
#define MSDOS_FAC (0x21)



typedef struct Rwblockm1
{ Byte      filler;
  Byte      func;
  Short     head;
  Short     cyl;
  Short     sect;
  Short     len;
  Byte *    buf;
} Rwblockm1_t, * Rwblockm1;




void explain()

{  fprintf(stderr, "dumpdisk -c # -h # -s #\n"
		   "         {-e} {-n}\n"
                   "	     {-f {#|#,#,#} {-b #} {-l #}{-/ \"string\"} {-v} -#\n");  
   fprintf(stderr, "   -e     Enquire of system for disk params\n"
                   "   -n     Say what you would do\n"
                   "   -f #   From sector #\n"
   		   "   -b #   by # tracks\n"
   		   "   -l #   output at most # tracks\n"
   		   "   -/ pat output only tracks containing pat\n"
   		   "   -v     verbose\n"
   		   "   -#     drive; 3=>c: #\n"
          );
                   
   exit(0);
}


#define SECTSZ 512
#define ATATIME 16


Short trash;

void wait(int n)

{ long nn = n * 10000;
  while (--nn > 0)
  { trash = 0;
  }
}



  Int cyl = 0;
  Int head = 0;
  Int sec = 0;

  Int clamp = 0x7fffffff;


local void dumpsect(Int foffs, Char * src)

{ Vint c = foffs / (head * sec);
  Vint hs = foffs - (Int)c * (head * sec);
  Vint h = hs / sec;
  Vint sect = hs - h * sec;
  Vint ix;

  if (--clamp < 0)
    return;
  
  printf(">>%d %d %d<<\n", c, h, sect);
  if (src != null)
  { for (ix = -1; ++ix < SECTSZ; )
      putc(src[ix], stdout);
    printf("\n");
  }
}



main(argc, argv)
    int argc;
    char ** argv;
{ if (argc <= 1)
    explain();

{ Int foffs = 0;
  Vint hd = -1;
  Vint sect = -1;
  Int flen = 0;
  Char * pattern = null;
  Bool v_opt = false;
  Bool e_opt = false;
  Bool n_opt = false;
  Vint drive = 1;
  
  Vint argsleft;
  Char ** argv_ = &argv[1];

  cyl = 0;
  head = 0;
  sec = 0;


  for (argsleft = argc; --argsleft > 0; )
  { if (argv_[0][0] != '-')
      break;

  { Char * s = &argv_[0][0];
    while (*++s != 0)
    { if      (*s == 'v')
        v_opt = true;
      else if (*s == 'e')
        e_opt = true;
      else if (*s == 'n')
        n_opt = true;
      else if (*s >= '0' and *s <= '9')
        drive = *s - '0';
      else if (*s == 'f')
      { ++argv_;
        foffs = atol(argv_[0]);
        hd = -1;
        sect = -1;
      { Vint ix;
        for (ix = 0; ++ix <= strlen(argv_[0]); )
          if (argv_[0][ix] == ',')
            if      (hd < 0)
              hd = atol(&argv_[0][ix+1]);
            else if (sect < 0)
              sect = atol(&argv_[0][ix+1]);
      }}
      else if (*s == 'b')
      { ++argv_;
        flen = atol(argv_[0]);
      }
      else if (*s == 'c')
      { ++argv_;
        cyl = atol(argv_[0]);
      }
      else if (*s == 'h')
      { ++argv_;
        head = atol(argv_[0]);
      }
      else if (*s == 's')
      { ++argv_;
        sec = atol(argv_[0]);
      }
      else if (*s == 'l')
      { ++argv_;
        clamp = atol(argv_[0]);
      }
      else if (*s == '/')
      { ++argv_;
        pattern = argv_[0];
        if (pattern[0] == '"')
        { pattern += 1;
          pattern[strlen(pattern)-1] = 0;		/* naughty */
        } 
      }
      else
        explain();
    }
    ++argv_;
  }}


  printf("-f %ld -b %ld -l %ld -/ %s -# %d\n", foffs, flen, clamp, pattern,drive);  

{ static Char buff[ATATIME * SECTSZ + 1];

  Vint progress = 0;  
  Int ix;
  Vint cur_atatime = ATATIME;

  if (e_opt)
  { rg.x.ax = 0x800;
    rg.h.dl = 0x80 + (drive-3);

    int86x(FDD_FAC, &rg, &rg, &segreg);
    if (rg.x.cflag != OK and rg.h.ah != 0)
      printf("Cannot query disk\n");
    else
    { if (cyl == 0)
        cyl = rg.h.ch + ((rg.h.cl & 0xc0) << 2) + 1;
      if (head == 0)
        head = rg.h.dh + 1;
      if (sec == 0)
        sec = (rg.h.cl & 0x3f) + 1;
      if (v_opt or flen == 0)
        printf("Cyls %ld Heads %ld Sectors %ld\n", cyl, head, sec);
    }
  }

  if (cyl == 0 or head == 0 or sec == 0)
    explain();

  if (hd >= 0)
    foffs = foffs * head * sec + hd * sec + sect;

  for (ix = flen; ix > 0; foffs += ATATIME)
  { Vint c = foffs / (head * sec);
    Vint hs = foffs - (Int)c * (head * sec);
    Vint h = hs / sec;
    Vint sect = hs - h * sec;
    Vint ct = 4;
    ix -= ATATIME;

    while (--ct >= 0 and not n_opt)
    { 
#if 0
      rg.x.ax = 0x200 + ATATIME;
      rg.h.ch = c & 255;
      rg.h.cl = ((c >> 2) & 0xc0) + sect;
      rg.h.dh = h;
      rg.h.dl = 0x80 + (drive - 3);
      rg.x.bx = (Int)&buff[0] & 0xf;
      segreg.es = (Int)&buff[0] >> 4;

      int86x(FDD_FAC, &rg, &rg, &segreg);
#else
      Rwblockm1_t dcr;
      Byte * ptr = &dcr.func;
      dcr.filler = 0;
      dcr.func = 0;
      dcr.head = h;
      dcr.cyl  = c;
      dcr.sect = sect;
      dcr.len  = ATATIME;
      dcr.buf  = buff;
      
      rg.x.ax = 0x440d;
      rg.x.bx = drive;
      rg.h.ch = 0x08;
      rg.h.cl = 0x61;
      rg.x.dx = FP_OFF(ptr); /* (Int)&buff[0] & 0xf; */
      segreg.ds = FP_SEG(ptr); /* (Int)&buff[0] >> 4; */

      int86x(MSDOS_FAC, &rg, &rg, &segreg);
#endif
      if (rg.x.cflag == OK)
        break;
      printf("Err. Code %d\n", rg.x.ax);
#if 1
      rg.x.ax = 0;
      int86x(FDD_FAC, &rg, &rg, &segreg);
#endif
      wait(10000);
    }

    if (ct < 0)
    { printf("Cannot read c %d h %d t %d\n", c, h, sect);
      continue;
    }
    /*
    if (n_opt or (++progress & 0x3fff) == 0)
      fprintf(n_opt ? stdout : stderr, "c %d h %d t %d\n", c, h, sect);
    */
    if (ix < 0)
      cur_atatime += ix;
    
    if (pattern == null)
    { Vint sect;
      for (sect = -1; ++sect < cur_atatime; )
      { Char * src = n_opt ? null : &buff[sect * SECTSZ];
        dumpsect(foffs+sect, src);
      }
    }
    else
    { if (n_opt)
      continue;
    
    { fast Char lead = *pattern;
           Vint sect;
      for (sect = -1; ++sect < cur_atatime; )
      { Char * src = &buff[sect * SECTSZ];
        Vint ct = SECTSZ;

        while (--ct >= 0)
        { if (*src == lead)
          { Char * src_ = src;
            Char * pat = pattern;
            while (*pat != 0 and *src_++ == *pat)
              ++pat;
            if (*pat == 0)
              dumpsect(foffs+sect, &buff[sect * SECTSZ]);
          }
          ++src;
        }
      }
    }}
  }
  

}}}


#endif
