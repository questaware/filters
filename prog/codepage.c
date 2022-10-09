#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


int main(int argc, char * argv[])

{ int old_cpi = GetConsoleCP();
  int old_cp = GetConsoleOutputCP();
	int cp = old_cp;
	if (argc > 1)
		cp = atoi(argv[1]);
	printf("Codepage in %d out %d\n", old_cpi, cp);
	if (cp != old_cp)
	{ // printf("set %d\n", cp);
		SetConsoleOutputCP(cp);
	}
{ int row, col;
	for (row = -1; ++row < 16; )
	{ printf("%1x: ", row);
		for (col = -1; ++col < 16; )
		{ putc(row*16+col, stdout);
		  putc(' ', stdout);
		}
		putc('\n', stdout);
	}
	
  if (cp != old_cp)
  	SetConsoleOutputCP(old_cp);
}}
