#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include "build.h"


#define SV_RD 45
#define SV_WR 46

void remove_queues(queues)
	Int queues[2];
{ msgctl(queues[SV_RD],IPC_RMID, 0);
  msgctl(queues[SV_WR],IPC_RMID, 0);
}



void setup_k_queues(queues, key)
	 Int     queues[2];
	 Int     key[2];
{ Bool qexists[2];
  queues[SV_RD] = msgget(key[SV_RD], 0777);
  qexists[SV_RD] = queues[SV_RD] >= 0;
  if (not qexists[SV_RD])
    queues[SV_RD] = msgget(key[SV_RD], 0777 | IPC_CREAT);
    
  queues[SV_WR] = msgget(key[SV_WR], 0777);
  qexists[SV_WR] = queues[SV_WR] >= 0;
  if (not qexists[SV_WR])
    queues[SV_WR] = msgget(key[SV_WR], 0777 | IPC_CREAT);

  if (queues[SV_RD] < 0 or queues[SV_WR] < 0)
  { fprintf(stderr, "msgget() failed. errno=(%d).", errno);
    exit(1);
  }
  fprintf(stderr, "ids: %ld %ld", queues[0], queues[1]);
}
