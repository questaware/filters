#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "build.h"
#include "lineqns.h"


#define STANDALONE 1



Vint get_soln(EqnDom arr[], Vint width, Vint ix)

{ Vint arrsz = width * (width + 1);
  Vint i;
  Vint ct = 0;
  Vint res;
  for (i = arrsz + ix + 1; (i -= width+1) > 0; )
    if (arr[i] != 0)
    { ++ct;
      res = arr[i - ix - 1];
    }
    
  if (ct > 1)
    fputs("INTERNAL ERROR", stdout);
  return res;
}





				/* -1 => no solution */
Cc solve_eqn(EqnDom arr[], Vint width)

{ Vint arrsz = width * (width + 1);
  Vint iter;
  Vint row;
  

  for (iter = width+1; --iter > 0; )
  { 			/* find a non zero */
    Vint cell, cell_, row = width;
    for (cell = iter+arrsz; (cell -= width+1) > 0 and arr[cell] == 0; )
      --row;
    if (cell <= 0)
      return -1;

  { Vint pval = arr[cell];
    Vint row_;
    
    for (row_ = width; --row_ >= 0; )
    { Vint rowcol = row_ * (width+1) + iter;
      if (rowcol != cell)
      { Vint step = cell - rowcol;
        Vint rowval = arr[rowcol];
        Vint ct = (width+1);
        for (cell_ = row_ * (width + 1); --ct >= 0; cell_ += 1)
          arr[cell_] = (arr[cell_] * pval - arr[cell_+step] * rowval) & 1;
      }
    }
  }}

  return OK;
}



#if STANDALONE

#define ARRSZ 5


Vint matrix[ARRSZ * (ARRSZ+1)] =
	       { 1,  1, 1, 0, 0, 0, 
		 0,  0, 1, 1, 1, 0, 
		 1,  1, 0, 0, 1, 0, 
		 1,  0, 1, 1, 0, 1, 
		 0,  1, 1, 1, 1, 0, 
	       };
Vint matrix_[ARRSZ * (ARRSZ+1)];



main ()

{ memcpy(&matrix_[0], matrix, sizeof(matrix_));
{ Cc cc = solve_eqn(matrix, ARRSZ);
  if (cc != OK)
    printf("Singular\n");
  else
  { printf("Solutions\n");
  { Vint i, j;
    for (i = -1; ++i < ARRSZ; )
      printf("%d\n", matrix[i * (ARRSZ+1)]);
   
    for (i = -1; ++i < ARRSZ; )
    { Vint res = 0;
      for (j = -1; ++j < ARRSZ; )
	res += matrix[j * (ARRSZ+1)] * matrix_[i * (ARRSZ+1)+1+j];
      if (res != matrix_[i * (ARRSZ+1)])
        printf("Discrepancies\n");
    }
  }}
}}


#endif
