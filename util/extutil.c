#include   <stdio.h>
#include   "build.h"

#if S_MSDOS
#include   <dos.h>
#include   <process.h>
#else 
#include   <dirent.h>
#endif


#include "../common/h/eprintf.h"


extern Char * this_ttype;

#define COMDIR "/etc/comments"

public Cc do_osys(cmd)
     Char * cmd;
{
#if S_MSDOS
  return shellprog(cmd);
#else 
  return system(cmd);
#endif
}



public Cc callprog(conc,cmd,argv)
        Bool   conc;
        Char * cmd;
        Char **argv;
{ tcapclose();
{ Id pid
#if S_MSDOS 
         = spawnv(P_WAIT, cmd, argv);
#else           
         = fork();
  if (pid == 0)
    execv(cmd, argv);
#endif

{ int rc = 0;
  Int pid_ = 
#if S_MSDOS
             pid;
  rc = pid;
#else             
             conc ? pid : wait(&rc);
#endif
  tcapopen(this_ttype);
  return pid == pid_ and rc == 0 ? OK : -1;
}}}        



public Int last_file(dir)
        Char *    dir;
#if S_MSDOS
{ 
}
#else
{ struct dirent * dp;
  Int last = 0;
  DIR * dirp = opendir(dir);
  if (dirp == null)
    return -1;

  while ((dp = readdir(dirp)) != null)
  { Int i = 0;
    sscanf(dp->d_name, "%d", &i);
    if (i > last)
      last = i;
  }
  
  closedir(dirp);
  return last;
}
#endif



public FILE * new_file(dir)
        Char *    dir;
{ Char buf[132];
  while (true)
  { Int last = last_file(dir);
    if (last < 0)
      return null;

    eprintf(&buf[0], "%s/%d", dir, last+1);
  { Id filix = open(buf, O_CREAT+O_EXCL+O_RDWR, 0644);
    if (filix >= 0)
    { FILE * fd = fdopen(filix, "w");
      if (fd != null)
        return fd;
    }
  }}
}

