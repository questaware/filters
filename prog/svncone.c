#include   <stdio.h>
#include   <ctype.h>
#include   <fcntl.h>
#include   "build.h"


static void explain()

{ fputs(
  "svncone <url> [<tmpdir>]\n",stdout
          );
  exit(0);
}

void main(argc, argv)
        int     argc;
        Char ** argv;
{ if (argc <= 1)
    explain();
  
{ char * url = argv[1];
  char * tdir;
  if (argc > 2)
    tdir = argv[2];
  else
    tdir = "tmpdir";
  
  if (argc > 1 && url[0] == '-') explain();

{ int sl = strlen(url);
  int ix;
  for (ix = sl; --ix > 0 && url[ix] != '/'; )
    ;
    
  if (ix == 0) explain();
  url[ix] = 0;
  
{ char buff[200];
  sprintf(buff,"svn co %s %s --depth empty", url,tdir);
  system(buff);
//  fputs(buff,stdout);fputs("\n",stdout);
  sprintf(buff,"svn up %s/%s", tdir,&url[ix+1]);
  system(buff);
//  fputs(buff,stdout);fputs("\n",stdout);
}}}}

