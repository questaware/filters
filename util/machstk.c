#include   <stdio.h>
#include   "build.h"


					/* code to test RS6000 AIX programe stack */

#define KEEP_STK  1

#define FRAMEOFFS (-6)
#define BIGGEST_STACK_ENY 5000
#define BIGGEST_STACK   100000

#define stack_diff(top,bot)   (STACK_GROWS_UP ? (Int)top - (Int)bot: \
                                                (Int)bot - (Int)top)



#if KEEP_STK

#define STK_SZ 50

 static Int ** call_fr_stk[STK_SZ];
 static Vint   cfs_top = -1;

#endif



void mstk_init()

{ 
#if KEEP_STK
  cfs_top = -1;
#endif
}



Cc mstk_sf_printstk( Int ** frame )

{ printf("F %x\n", frame);
  printf("%8x %8x %8x %8x %8x %8x %8x %8x\n", frame[-8], frame[-7], frame[-6], frame[-5],
  				              frame[-4], frame[-3], frame[-2], frame[-1]);
  printf("%8x\n", frame[0]);
  printf("         %8x %8x %8x %8x %8x %8x %8x\n", frame[1], frame[2], frame[3], frame[4],
  				                    frame[5], frame[6], frame[7]);

}




Int * * mstk_sf_next( Int * * bframe )

{ Int * * next =  bframe == null ? null : (Int**)bframe[0];
  Int diff = stack_diff(bframe, next);
  
  return ((unsigned)(diff-1) > BIGGEST_STACK_ENY) ? null : next;
}



Int * * mstk_prep_sf_first(long a)

{ return mstk_sf_next(((Int**)&a) + FRAMEOFFS);
}




void mstk_dump(long a)

{ Int  ** frame = ((Int**)&a) + FRAMEOFFS;

  while (frame != null and ((long)frame & 3) == 0)
  { mstk_sf_printstk(frame);
    frame = mstk_sf_next(frame);
  }
}





static Vint find_frame(Int ** frame)

{ Vint ix;
  Vint res = -1;

  for (ix = cfs_top; ix >= 0; --ix)
    if (((Int)call_fr_stk[ix] & -2) == (Int)frame)
      res = ix;
      
  return res;
}


#if KEEP_STK

void mstk_dump_calls( char * msg)

{ Vint i;
  
  
  printf("%s\n", msg);
  
  for (i = cfs_top+1; --i >= 0; )
    printf("Addr %8x   %d\n", call_fr_stk[i], (int)call_fr_stk[i] & 1);
}


							/* returns number of noted frames */
Vint mstk_note(Int a)

{ Int  ** frame = ((Int**)&a) + FRAMEOFFS;
  Int  ** frame_ = frame;
  Int diff;
  Vint gained_frames = 0;
  Vint noogood = 0;
  Vint lasttop = -1;

  frame = mstk_sf_next(frame);
  
  for (; (diff = stack_diff(frame_, frame)) < BIGGEST_STACK and
          frame != null and ((long)frame & (LONG_ALGNMT-1)) == 0; )

  for (; (diff = stack_diff(frame_, frame)) < BIGGEST_STACK and frame != null and ((long)frame & 3) == 0; )
  {
    if (upper_index(call_fr_stk) - gained_frames > cfs_top)
    { ++gained_frames;
      printf("Gain at %d %x\n", upper_index(call_fr_stk) - gained_frames, frame);
      call_fr_stk[upper_index(call_fr_stk) - gained_frames] = frame;
    }
    
  { Vint fr = find_frame(frame);
    if (fr >= 0 and lasttop < 0)
    { lasttop = fr;
      noogood = gained_frames-1;
    }

    frame = mstk_sf_next(frame);
  }}

{ Vint noted_frames = noogood > 0;
  Vint i;
  for (i = lasttop+1; --i >= 0; )
    if (*(Int*)&call_fr_stk[i] & 1)
      noted_frames += 1;

  if (lasttop < 0)
  { printf("Resetting stack\n");
    noogood = gained_frames;
    noted_frames = 1;
  } 

  printf("Kept Frames %d New frames %d\n", lasttop+1, noogood);
  memmove(&call_fr_stk[lasttop+1],
          &call_fr_stk[upper_index(call_fr_stk) - noogood],
          noogood * sizeof(Byte*));
  cfs_top = lasttop + noogood;

  *(Int*)&call_fr_stk[cfs_top] |= 1;			/* mark it */

  (void)mstk_dump_calls("===");
  return noted_frames;
}}


#endif
