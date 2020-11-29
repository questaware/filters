#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <resolv.h>
#include "version.h"




Char dbg_file_name[] = "notused";

ps_dump() {}




void main(argc,argv)
	Vint argc;
        Char ** argv;
{ Vint argsleft = argc - 1;
  Char ** argv_ = &argv[1];

  int sid = socket(AF_INET, SOCK_STREAM, 0/*IPPROTO_UDP*/);

  struct sockaddr_in addr_sin;
  if (sid < 0)
  { fprintf(stderr, "Cannot create socket");
    exit(1);
  }
  
  memset(&addr_sin, 0, sizeof(struct sockaddr_in));
  addr_sin.sin_family = AF_INET;
  addr_sin.sin_port = htons(0x41b);
  addr_sin.sin_addr.s_addr = htonl(INADDR_ANY);
 
{ int cc = bind(sid, (void*)&addr_sin, sizeof(addr_sin));
  if (cc < OK)
  { fprintf(stderr, "Bind failed %d\n", errno);
    exit(1);
  }  
  
{ union 
  { Char               b[256];
    struct sockaddr_in sin; 
  } u;
  int salen = 256;
  
  Cc cc = getsockname(sid, (void*)&u, &salen);
  if (cc != OK)
  { fprintf(stderr, "gsn failed %d\n", errno);
    exit(1);
  }
  
  if (u.sin.sin_family != AF_INET)
  { fprintf(stderr, "Is not AF_INET %d\n", u.sin.sin_family);
    exit(1);
  }
  
  printf("Port %x Addr %x\n", u.sin.sin_port, u.sin.sin_addr.s_addr);
  
  
  exit(0);
}}}

