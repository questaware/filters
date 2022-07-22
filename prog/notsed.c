#include <stdio.h>

int main()
{ char line[132];
	char * ln;
  while ((ln = fgets(line, sizeof(line)-1, stdin)))
  { ln[strlen(ln)-1] = 0;
  	printf("\"%s\\n\"", ln);
	}
	return 0;
}
