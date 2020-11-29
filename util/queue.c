#if 0
  logmsg missing
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
#include "../../h/defs.h"
#include "../../h/logmsg.h"


#define E_RD  1
#define E_WR  0

public void remove_queues(queues)
	Int queues[2];
{ msgctl(queues[SV_RD],IPC_RMID, 0);
  msgctl(queues[SV_WR],IPC_RMID, 0);
}



public void setup_queues(queues, cmdnm, queuesz)
	 Int     queues[2];
	 Char *  cmdnm;
	 Int     queuesz;
{ Int i;
  Bool  qexists[2];
  key_t key[2];			    /* get queue id from the executable file */
  key[SV_RD] = ftok(cmdnm, (char)SV_RD + 1);
  key[SV_WR] = ftok(cmdnm, (char)SV_WR + 1);
  if (key[SV_RD] == (key_t)(-1) or key[SV_WR] == (key_t)(-1))
  { logmsg(Q_LOG_SEVERE, "ftok() failed on %s, errno=(%d).", cmdnm, errno);
    exit(1);
  }
  
  queues[SV_RD] = msgget(key[SV_RD], 0777);
  qexists[SV_RD] = queues[SV_RD] >= 0;
  if (not qexists[SV_RD])
    queues[SV_RD] = msgget(key[SV_RD], 0777 | IPC_CREAT);
    
  queues[SV_WR] = msgget(key[SV_WR], 0777);
  qexists[SV_WR] = queues[SV_WR] >= 0;
  if (not qexists[SV_WR])
    queues[SV_WR] = msgget(key[SV_WR], 0777 | IPC_CREAT);

  if (queues[SV_RD] < 0 or queues[SV_WR] < 0)
  { logmsg(Q_LOG_SEVERE, "msgget() failed. errno=(%d).", errno);
    exit(1);
  }
  logmsg(Q_LOG_INFO, "Name %s, ids: %ld %ld", cmdnm, queues[0], queues[1]);

{ struct msqid_ds buf;
  (void)msgctl(queues[0], IPC_STAT, &buf);
  buf.msg_qbytes = queuesz / 2;
  logmsg(Q_LOG_INFO, "RD sz: %d ", buf.msg_qbytes);
{ Cc cc = qexists[0] ? OK : msgctl(queues[0], IPC_SET, &buf);
  if (cc != OK)
    logmsg(Q_LOG_SEVERE, "Error setting queue1 size");

  (void)msgctl(queues[1], IPC_STAT, &buf);
  buf.msg_qbytes = queuesz / 2;
  logmsg(Q_LOG_INFO, "WR sz: %d ", buf.msg_qbytes);
  cc = qexists[1] ? OK : msgctl(queues[1], IPC_SET, &buf);
  if (cc != OK)
    logmsg(Q_LOG_SEVERE,"Error setting queue2 size");
}}}

#endif
