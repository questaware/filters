#include <stdio.h>
#include "build.h"
#include "../common/h/eprintf.h"

void fatal(void) { }

char buf[80];

p() { fputs(buf, stdout); }


main()

{ eprintf(buf, "2d 123 %2d\n", 123);
  p();
  eprintf(buf, "02d 123 %02d\n", 123);
  p();
  eprintf(buf, "3d 123 %3d\n", 123);
  p();
  eprintf(buf, "03d 123 %03d\n", 123);
  p();
  eprintf(buf, "3d -123 %3d\n", -123);
  p();
  eprintf(buf, "04d -123 %04d\n", -123);
  p();

  eprintf(buf, "04x 0x1234 %04x\n", 0x1234);
  p();
  
  eprintf(buf, "04x 0xabcd %04x\n", 0xabcd);
  p();

  eprintf(buf, "lx 0x12345678 %lx\n", 0x12345678);
  p();
  eprintf(buf, "lx 0xabcdef01 %lx\n", 0xabcdef01);
  p();
}
