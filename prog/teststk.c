#include <stdio.h>
#include <stdarg.h>
#include "build.h"

#include "../util/machstk.h"


struct Sct
{ int a;
  int b;
};


struct Sct *  psct[5];
       Byte * rastk[5];




#define d(i) ((int)psct[i] + 24)
#define e(i) rastk[i]

#if 0

void my_debugf(char *Format_cp, ...)

{ Vint dpth = mstk_note(0)-1;
  va_list Args_t;
  va_start(Args_t, Format_cp);

  printf("-------------------");
  printf(&"                    "[20-dpth*2]);

  vfprintf(stdout, Format_cp, Args_t);
  printf("\n");
  
  fprintf(stderr, &"                    "[20-dpth*2]);

  vfprintf(stderr, Format_cp, Args_t);
  fprintf(stderr, "\n");

  va_end(Args_t);
}




arm2()

{ 
  my_debugf("--arm2");
  mstk_dump_calls("arm2");
}




arm1()

{ my_debugf("< arm1");
    arm2();
  my_debugf("> arm1");
}




pa(int a)		/* 3456789a */

{ struct Sct pa_s;  psct[0] = &pa_s;
  my_debugf("> pa");

  rastk[0] = (Byte*)pa;
  printf("PA %8x PB %8x PC %8x M %8x\n\n",
          d(0), d(1), d(2), d(3)
        );

  printf("RA %8x RB %8x RC %8x M %8x\n\n",
          e(0), e(1), e(2), e(3)
        );

  mstk_dump(a + 0x11111111); /* 456789ab */

  mstk_dump_calls("pa");
  my_debugf("< pa");
}




pb(int b)		/* 23456789*/

{ struct Sct pb_s;   psct[1] = &pb_s;
  my_debugf("> pb");
  rastk[1] = (Byte*)pb;
   mstk_dump_calls("pb");
   pa(b + 0x11111111);
  my_debugf("< pb");
}




pc(int c)

{ struct Sct pc_s;   psct[2] = &pc_s;
  my_debugf("> pc");
  rastk[2] = (Byte*)pc;
   pb(c + 0x11111111);
   arm1();
  my_debugf("< pc");
}

#endif

main()

{ int cix;
  for (cix = (int)'0'; ++cix < 256; )
  { printf("char %x %c.\n", cix, (char)cix);
	}
#if 0
{ struct Sct pm_s;   psct[3] = &pm_s;
  rastk[3] = (Byte*)main;
  my_debugf("main begin");
  mstk_dump_calls("main");
  pc(0x12345678);
}
#endif
}
