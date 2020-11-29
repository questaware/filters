#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "build.h"

extern Char * getenv();
extern Char * malloc();
extern Int atol();


Bool v_opt = false;
Bool w_opt = false;
Vint  e_ix = -1;
Vint  ec_log_size = 8;
Int   ec_ext_sz = 0;

Vint seed = 7;


Byte mask_map0[0x8000L];
Byte mask_map1[0x8000L];

Byte check_sum[40];
Byte cover[40];			/* number of errors in this */


static Int randomm = 654654564;

#define PRIME (2 * 3 * 4 * 5 * 6 * 7 - 1)

static Int gen_randm()

{ Int r = randomm * PRIME;
  randomm = (r >> 4) ^ r;
  r = randomm >> 4;
  return r;
}



private void explain()

{ fprintf(stderr, "ec [-e #] [-s #] [-i #] [-r|-w] filename\n"
                  "   -r -- Read data (default)\n"
                  "   -w -- Write data\n"
                  "   -e -- Simulate Error at #\n"
                  "   -i -- initial random number seed\n"
                  "   -s -- Log2(size of area)\n"
                  "   -x -- extension size\n"
                  "   -d -- number of primes to chop\n"
                  "   -v -- Verbose\n" );
  exit(0);
}

Vint lo_primes[16]={3, 5, 17, 29, 37, 53, 131, 167, 193, 383, 577, 773, 1531, 
			1999,2617, 3083, };

Vint prm12[]={23, 29, 37, 53, 131, 167, 193, 293, 383, 577, 773, 1097, 1531,};
							   /* 47*61 */
Vint prm13[]={3,23,37,53,107,167,347,773,1097,1531,1999,2411,4391, 6143, };

Vint prm14[]={3,23,37,53,107,167,347,773,1097,1531,1999,2411,4391, 6143, 12289,};

Int yield[32/*is big enough */];

Vint errors[] = { 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 , 1, 1};


void generate_matrix(Vint log_size, Int width)


{ /* Int width = (1L << log_size); */
  Int asz = (width >> 3)  * (log_size + 1);
  Vint ix, iy;
    
  if (asz > 0x8000L)
  { printf("Map Too Big at %lx\n", asz);
    exit(7);
  }

  memset(&mask_map0[0], 0, sizeof(mask_map0));
  memset(&mask_map1[0], 0, sizeof(mask_map1));

  for (iy = -1; ++iy <= log_size; 0)
  {
    for (ix = width; (ix -= 4) >= 0; )
    { unsigned int i = ((Int)iy * width + ix) >> 3;
      mask_map0[i] = (~(ix >> iy) 	        & 1)	   |
		    ((~((ix+1) >> iy)	        & 1) << 1) |
		    ((~((ix+2) >> iy)	        & 1) << 2) |
		    ((~((ix+3) >> iy)	        & 1) << 3);
     mask_map1[i] =  (~(ix / lo_primes[iy])     & 1)        |
		     ((~((ix+1) / lo_primes[iy]) & 1) << 1) |
		     ((~((ix+2) / lo_primes[iy]) & 1) << 2) |
		     ((~((ix+3) / lo_primes[iy]) & 1) << 3) |
		     ((~((ix+4) / lo_primes[iy]) & 1) << 4) |
		     ((~((ix+5) / lo_primes[iy]) & 1) << 5) |
		     ((~((ix+6) / lo_primes[iy]) & 1) << 6) |
		     ((~((ix+7) / lo_primes[iy]) & 1) << 7);
    }
  }
}



Vint count_crosses(Byte counts_ref[], Vint errix[4], Vint log_width, Int width)

{ Vint odds = 0;
  Vint iter;
  Vint iy;

  for (iy = log_width + 1; --iy >= 0; )
  { Int cell = (Int)iy * width;
    Vint ctodd = 0;
    Vint cteven = 0;
    unsigned int ixs[4];
    ixs[0] = (cell + errix[0])>> 3;
    ixs[1] = (cell + errix[1])>> 3;
    ixs[2] = (cell + errix[2])>> 3;
    ixs[3] = (cell + errix[3])>> 3;
    counts_ref[iy*2+1] = 0;

    ctodd ^= mask_map1[ixs[0]] >> (errix[0] & 7);
    ctodd ^= mask_map1[ixs[1]] >> (errix[1] & 7);
    ctodd ^= mask_map1[ixs[2]] >> (errix[2] & 7);
    ctodd ^= mask_map1[ixs[3]] >> (errix[3] & 7);
  /*counts_ref[iy*2] = ct & 1;
    odds |= ct & 1;
   */
    if (ctodd & 1)
    { counts_ref[iy*2+1] = 1;
      odds |= 0x8000;
    }
  }
  
  return odds;
}

void ec(Vint errs[4])

{ 					/* generate checksums */
/*
  Vint ix, ic;
  memset(check_sum, 0, sizeof(check_sum));
  for (ic = ec_log_size*2 -1; --ic >= 0; )
    for (ix = 4; --ix >= 0; )
      if (mask_map[ic * ec_size + errs[ix]])
        check_sum[ic] ^= 1;
  for (ic = ec_log_size; --ic >= 0; )
  { Vint mask = (1 << ic);
    Vint sum = 0;
    Vint uniq[2]; uniq[0] = 0;
    		  uniq[1] = 0;
    
    for (ix = 4; --ix >= 0; )
    { if (errs[ix] & mask)
      { sum += 1;
        uniq[1] = ix;
      }
      else
        uniq[0] = ix;
    }
  }
*/
  memset(cover, 0, sizeof(cover));
}


Int pow3[] = {
1,
3, 
3*3, 
3*3*3, 
3L*3*3*3, 
3L*3*3*3*3, 
3L*3*3*3*3*3,
3L*3*3*3*3*3*3,
3L*3*3*3*3*3*3*3,
3L*3*3*3*3*3*3*3*3,
3L*3*3*3*3*3*3*3*3*3,
3L*3*3*3*3*3*3*3*3*3*3,
3L*3*3*3*3*3*3*3*3*3*3*3,
3L*3*3*3*3*3*3*3*3*3*3*3*3,
3L*3*3*3*3*3*3*3*3*3*3*3*3*3,
3L*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
3L*3*3*3*3*3*3*3*3*3*3*3*3*3*3*3,
};

Vint combs8[] = {3, 5, 6 };

Int comb_ord = -1;

Vint comb_lw;
Vint comb_lwm1;

void init_comb(Vint log_width)

{     comb_lw = log_width;
  if (comb_lw > upper_index(pow3) or comb_lw > upper_index(lo_primes))
  { printf("Too Big\n");
    exit(1);
  }
  comb_lwm1 = comb_lw - 1;
}



Cc gen_next_comb(Vint errix[4])

{ Vint topbit = (1 << comb_lwm1);
  while (true)
  { errix[0] = 0;
    errix[1] = 0;
    errix[2] = topbit;
    errix[3] = topbit;
    comb_ord += 1;
    if (comb_ord >= pow3[comb_lwm1] + ec_ext_sz)
      return -1;
      
  { Int lix = comb_ord;
    Vint bit = 1;
    Vint slice;
    for (slice = comb_lwm1; --slice >= 0; bit = bit << 1)
    { Vint slix = lix / pow3[slice];
      Vint pair = combs8[slix];

      if (slix > 2)
	printf("SHIT");
      if (pair & 1)
	errix[0] |= bit;
      if (pair & 2)
	errix[1] |= bit;
      if (pair & 4)
	errix[2] |= bit;
      if (pair & 8)
	errix[3] |= bit;
  
      lix -= slix * pow3[slice];
    }
    if (errix[0] < errix[1])
      break;
  }}  
  return OK;
}

void main(argc, argv)
        Vint    argc;
        Char ** argv;
{ if (argc <= 1)
  { explain();
  }
  
{ Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];

  for (; argsleft > 0 and argv_[0][0] == '-'; --argsleft)
  { Char * flgs;
    for (flgs = &argv_[0][0]; *++flgs != 0; )
      if      (*flgs == 'r')
        w_opt = false;
      else if (*flgs == 'v')
        v_opt = true;
      else if (*flgs == 'w')
        w_opt = true;
      else if (*flgs == 'e')
      { if (in_range(flgs[1], '0','9'))
        { e_ix = atoi(&flgs[1]);
          flgs = "x";
        }
        else
        { e_ix = atoi(argv_[1]);
          --argsleft;
          ++argv_;
        }
        
      }
      else if (*flgs == 's' or *flgs == 'x')
      { Vint val;
        if (in_range(flgs[1], '0','9'))
        { val = atoi(&flgs[1]);
          flgs = "x";
        }
        else
        { val = atoi(argv_[1]);
          --argsleft;
          ++argv_;
        }
        if (*flgs == 's')
          ec_log_size = val;
        else
          ec_ext_sz = val;
      }
      else
        explain();
    ++argv_;
  }

  if (argsleft < 1)
  { explain();
  }

{ Char * filename = argv_[0];
  Int times = atol(filename);
  Vint log_wdth = ec_log_size;

  if (log_wdth >= 12)
    if	    (log_wdth == 12)
      memcpy(&lo_primes[0], prm12, sizeof(lo_primes));
    else if (log_wdth == 13)
      memcpy(&lo_primes[0], prm13, sizeof(lo_primes));
    else if (log_wdth == 14)
      memcpy(&lo_primes[0], prm14, sizeof(lo_primes));
    else
      log_wdth = 24;	/* too big */
  
  generate_matrix(log_wdth, (1 << log_wdth)+ec_ext_sz);
  init_comb(log_wdth);

{ Vint maxlow = 0;
  Byte * counts = (Byte*)malloc(log_wdth*2+2);
  Vint errix[4];
  Int samples = 0;
  Vint sings = 0;

  while (--times >= 0)
  { Vint i;
    if (false  and log_wdth != 8)
      for (i = 4; --i >= 0; )
        errix[i] = gen_randm() & ((1 << log_wdth) - 1);
    else
      if (gen_next_comb(&errix[0]) != OK)
      { printf("DONE\n\n");
        break;
      }

    ++samples;

  { Vint odds = count_crosses(counts, errix, log_wdth, (1<<log_wdth)+ec_ext_sz);
    if (log_wdth <= 10 or (Short)odds >= 0)
    { printf("%d %d %d %d --%d\n", 
              errix[0], errix[1], errix[2], errix[3], maxlow);
    /*if (odds & 1)
        printf("oh\n");*/
    }
    if (true or v_opt)
    { Vint lowest = -1;
      for (i = -1; ++i <= log_wdth; ) 
      { /* if (counts[i*2] & 1)
          printf("OH(%d)", i);*/
       if (v_opt and log_wdth <= 10 or (Short)odds >= 0)
          printf("%d  %d\n", 
		  lo_primes[i], counts[i*2+1] & 1);
	if (lowest < 0 and (counts[i*2+1] & 1))
	  lowest = i;
	if (counts[i*2+1] & 1)
	  yield[i*2] += 1;
      }
      if (lowest > maxlow)
        maxlow = lowest;
    }
    if ((Short)odds >= 0)
    { printf(" NO ODDS\n");
      sings += 1;
      /* break; */
    }
 /* if (odds & 1)
      printf("--OH--\n"); */
  }}

  printf("Size %d Samples %ld Singularities %d\n", (1<<log_wdth), samples, sings);
{ Vint i;
  for (i = -1; ++i <= log_wdth; ) 
  { printf("%d %ld\n", lo_primes[i], yield[i*2]);
  }

}}}}}

