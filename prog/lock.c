#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <fcntl.h>
#include "build.h"

#include "../common/h/eprintf.h"



#define private

#define DFLT_LOCKLN 20
#define DFLT_LOCKFN "/tmp/LK542957"

#define Time Int

#define Q_LOCK   1
#define Q_UNLOCK 2

#define RW_ME 0600

static struct sembuf sops;

static Id semid;

Bool rang = false;

Char line[255];


#define LOCKREC_SZ 80

Char lock_rec[LOCKREC_SZ];


Int that_ppid;
Int that_expiry;
Int that_lklen;
Int that_seq;

extern Char * ctime(), *strchr();

private Char * c_time()

{ Time now = time(null);
  Char * res = ctime(&now);
  Char * sp = res;
  Char * sp_;
  Char * sp__;
  
  while ((sp = strchr(sp, ' ')) != null)
  { sp = &sp[1];
    sp__ = sp_;
    sp_ = sp;
  }
  
  sp_[0] = 0;
  return sp__;
}



private void i_log(fmt, a, b, c, d)
	 Char * fmt;
{ printf("%s : ", c_time());
  printf(fmt, a, b, c, d);
  putc('\n', stdout);
  fflush(stdout);
}


private void null_proc(f, a, b, c, d) { }

#if DEBUG <= 1
# define n_log null_proc
#else
# define n_log i_log
#endif

Cc mk_sem(semkey)
	Id   semkey;
{ semid = semget((long)semkey, 2, IPC_CREAT | IPC_EXCL | RW_ME);
{ Bool first = semid >= 0;

  if (not first)
  { semid = semget((long)semkey, 2, IPC_CREAT | RW_ME);
    if (semid < 0)
    { i_log("Cannot access Semaphore");
      exit(1);
    }
  }
  else
  { Int i;
    n_log("I am the first");
    
    for (i = 1; i >= 0; --i)
    { Cc cc = semctl(semid, i, (long)SETVAL, 1L);
      if (cc != OK)
        i_log("cache sem init"); 
      cc = (1 - semctl(semid, i, (long)GETVAL, 0L));
      if (cc != 0)
        i_log("E init c sem %d, err %d", cc, errno);
    }
  }
  
  return OK;
}}

Bool tx_lock(wh, n)
	Quot wh;
{ Cc cc;
  rang = false;
  sops.sem_num = n;
  sops.sem_flg = 0;
  
  if (wh == Q_LOCK)
  { sops.sem_op = -1;
    while ((cc = semop(semid, &sops, 1L)) == -1)
    { if (errno == EAGAIN or errno == EINTR and rang)
        return false;
      i_log("Error in sop %d\n", errno);
      exit(1);
    }
    n_log("QXC(%d) += %d", n, 1 - semctl(semid, n, (long)GETVAL, 0L));
  }
  else
  { sops.sem_op  = 1;
    cc = semop(semid, &sops, 1L);
    if (cc < 0)
    { i_log("Error in sop %d\n", errno);
      exit(1);
    }
    n_log("QXC(%d) -= %d", n, 1 - semctl(semid, n, (long)GETVAL, 0L));
  }
  return true;
}


  
private void ring()

{ signal(SIGALRM, ring);
  rang = true;
}



private Bool rd_details(fd)
	 Id   fd;
{ lseek(fd, 0, 0);
  if (not read(fd, &lock_rec[0], LOCKREC_SZ))
    return false;

  that_ppid = 0;
  sscanf(lock_rec,"%d %d %d %d",&that_ppid,&that_expiry,&that_lklen,&that_seq);
  return that_ppid != 0;
}


private void wr_details(fd, ppid, expiry, lklen, seq)
	 Id   fd;
{ lseek(fd, 0, 0);
  eprintf(lock_rec,"%d %d %d %d", ppid, expiry, lklen, seq);
  if (write(fd, &lock_rec[0], LOCKREC_SZ) != LOCKREC_SZ)
    i_log("WRITEFAIL");
}

public main(argc, argv)
	Int      argc;
	Char * * argv;
{ Int locklen =   argc < 2 ? DFLT_LOCKLN : atoi(argv[1]);
  Char * lockfn = argc < 3 ? DFLT_LOCKFN : argv[2];
  Id this_ppid = argc < 4 ? getppid() : atoi(argv[3]);
  Id semkey = ftok(lockfn, 4);  /* say */
  Char * cmdstr_ = argv[0];
  Char * cmdstr;
  Bool lock = true;
  Bool broke_lock = false;
  
  for (cmdstr = cmdstr_; *cmdstr != 0; ++cmdstr)
    if (*cmdstr == 'u' and strcmp(cmdstr, "unlock") == 0)
    { lock = false;
      break;
    }

  if (locklen == 0)
  { fprintf(stderr, "Usage: %s [ seconds [ file [ ppid ] ] ]", cmdstr_);
    exit(0);
  }

  eprintf(&line[0], "/tmp/pjs%d", this_ppid);
  freopen(line, "a", stdout);
  signal(SIGALRM, ring);

  i_log(lock ? "Lock" : "Unlock");
  mk_sem(semkey);

{ Id lkfd = open(lockfn, O_RDWR | O_CREAT, 0755);
  if (lkfd < 0)
  { i_log("Couldnt open lock file %s %d", lockfn, errno);
    exit(1);
  }
  if (not lock)
  { if (not rd_details(lkfd) or this_ppid != that_ppid)
      i_log("Warning: unlocking too late");
    else
    { i_log("Unlocked");
      tx_lock(Q_UNLOCK, 0);
    }
  }
  else
  { Int left = 4;
    (void)tx_lock(Q_LOCK, 1);
  
    while (true)
    { alarm(left);
    { Bool did = tx_lock(Q_LOCK, 0);
      Time now = time(null);
      Bool didr = rd_details(lkfd);
      alarm(0);
      if (did)
      { wr_details(lkfd, this_ppid, time(null) + locklen, locklen, that_seq+1);
	i_log("Locked");
        break;
      }
      left = not didr ? 5 : that_expiry - now;
      if (left <= 0)
      { i_log("Breaking lock of len %d", that_lklen);
        rd_details(lkfd);
        wr_details(lkfd, this_ppid, time(null) + locklen, locklen, that_seq+1);
        left = 4;
        break;
      }
      left = not didr ? 5 : that_expiry - now;
      if (left < 2)
        left = 2;
    }}
    (void)tx_lock(Q_UNLOCK, 1);
  }
  
}}
