#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include "../h/base.h"


#define UNAME "peter"

Cc gotsignal()

{ signal(SIGHUP, gotsignal);
  fprintf(stderr, "Got signal");
}



main ()

{ struct passwd * pw = getpwnam(UNAME);
  if (pw == null)
  { fprintf(stderr, "no passwd entry\n");
    exit(1);
  }
  
  signal(SIGHUP, gotsignal);
    
{ Id suid = getuid();
  Id client = pw->pw_uid;
  
  Cc cc = setuid(client);
  if (cc != OK)
  { fprintf(stderr, "Fail1 %d %d\n", cc, errno);
    exit(1);
  }
  
  fprintf(stderr, "Real %d Effective %d\n", getuid(), geteuid());
  sleep(60);
  
{ Cc cc = setuid(suid);
  if (cc != OK)
  { fprintf(stderr, "Fail2 %d %d\n", cc, errno);
    exit(1);
  }
  
  fprintf(stderr, "Real %d Effective %d\n", getuid(), geteuid());
}}}
