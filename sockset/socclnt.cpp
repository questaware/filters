#include <windows.h>
#include <stdio.h>
#include <winsock.h>
#include <io.h>
#include <stdlib.h>


#include "build.h"


extern "C" {
#include "../util/h/strutil.h"
}


//__declspec(noreturn) void   __cdecl exit(int);


// Global Variables:
#define MAX_LOADSTRING 60

TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

#if USE_CONTCP
#define SOCTYP SOCK_STREAM
#else
#define SOCTYP SOCK_DGRAM
#endif



#define SOCKNAME "2100"


Char dbg_file_name[] = "notused";

#define BUFLEN 513


Char  filen[257];
Char  in_buf[BUFLEN + sizeof(Int)];
Char out_buf[BUFLEN + sizeof(Int)];



typedef union soc
{ struct sockaddr    un;
  struct sockaddr_in in;
  Char               buf[2+256];
} Soc_addr;

Soc_addr soc_addr;


ps_dump() { return 0;}


static void explain()

{ fprintf(stderr, "socclnt [ [#][@server] | file ]\n"
                  "     #      port number to offer   DEFAULT 2100\n"
                  "     server name of server machine DEFAULT localhost\n"
                  "     file   name of socket\n"
      	       );
  exit(0);
}

void client_req(Char type, Char * msg)

{ out_buf[sizeof(Int)-1] = type;
  strpcpy(&out_buf[sizeof(Int)], msg, sizeof(out_buf)- sizeof(Int));
}




void client_reply(Char in_buf[], Int got)

{ printf("Type %c: %s\n", in_buf[sizeof(Int)-1], &in_buf[sizeof(Int)]);
}

// Foward declarations of functions included in this code module:


int main( int argc, char * argv[] )

{ Id port = 0;
  Id addr = -1;
     
  Vint argsleft;
  Char ** argv_ = &argv[1];

  for (argsleft = argc; --argsleft > 0; )
  { Char * s = &argv_[0][0];

    while (*s != 0)
    { if (in_range(*s, '0', '9'))
      { port = atol(s);
	while (in_range(*s, '0', '9'))
	  ++s;
      }
      else if (port > 0 and *s == '@')
      { Int ix = -1;
	while (*++s != 0)
	  in_buf[++ix] = *s;
	in_buf[++ix] = 0;
      }
      else
      { Int ix = -1;
	while (*++s != 0)
	{ if (not isalpha(*s) and not in_range(*s,'0', '9') and
					       *s != '_') 
	  { explain();
	  }
	  filen[++ix] = *s;
	}
	filen[++ix] = 0;
      }
    }

    ++argv_;
  }

  if (port > 0 and in_buf[0] == 0)
  { strcpy(&in_buf[0], "127.0.0.1"); 
  }	

  if (filen[0] == 0 and port <= 0)
    port = 2100;
    
  if (filen[0] != 0)
  { fprintf(stderr, "File Sockets not supported\n");
    exit(1);
  }

  WORD wVersionRequested;WSADATA wsaData;int err; 
  wVersionRequested = MAKEWORD( 1, 1 ); 
  err = WSAStartup( wVersionRequested, &wsaData );
  if ( err != 0 ) 
  { /* Tell the user that we could not find a usable */
    /* WinSock DLL.                                  */    
    exit(1);
  } 
    /* Confirm that the WinSock DLL supports 2.2.*/
    /* Note that if the DLL supports versions greater	 */
    /* than 2.2 in addition to 2.2, it will still return */
    /* 2.2 in wVersion since that is the version we	 */
    /* requested.					 */ 
  if ( LOBYTE( wsaData.wVersion ) != 1 ||
        HIBYTE( wsaData.wVersion ) != 1 ) {
    /* Tell the user that we could not find a usable */
    /* WinSock DLL.                                  */    
    WSACleanup( );
    exit(1); 
  }

{ SOCKET sid = socket(port >= OK ? AF_INET : AF_UNIX, SOCTYP, 0/*IPPROTO_UDP*/);
  if (sid < 0)
  { fprintf(stderr, "Cannot create socket\n");
    exit(1);
  }
  
  memset(&soc_addr, 0, sizeof(struct sockaddr_in));
  soc_addr.in.sin_family = AF_INET;
  soc_addr.in.sin_addr.s_addr = htonl(INADDR_ANY);
    
{ int cc = bind(sid, &soc_addr.un, sizeof(soc_addr.in));
  if (cc < OK)
  { int errno = GetLastError();
    fprintf(stderr, "Bind failed %d\n", errno);
    exit(1);
  }  

{ int addrlen;

  if (port < 0)
  { soc_addr.un.sa_family = AF_UNIX;
    strpcpy(&soc_addr.un.sa_data[0], filen, sizeof(soc_addr.un.sa_data)); 
    addrlen = sizeof(soc_addr);
  }
  else
  { memset(&soc_addr, 0, sizeof(soc_addr));

  { int cc = inet_addr(in_buf[0] == 0 ? "127.0.0.1" : in_buf);
    if (cc == -1)
    { fprintf(stderr, "Cannot find address (%d:%d) %s\n", cc, errno,
                        in_buf[0] == 0 ? "127.0.0.1" : in_buf);
      exit(1);
    }

    soc_addr.in.sin_family = AF_INET;
    soc_addr.in.sin_port = htons((unsigned short)port);
    soc_addr.in.sin_addr.s_addr = cc;
    addrlen = sizeof(soc_addr.in);
  }}
    
{ int ix;
#if USE_CONTCP
  Cc cc = connect(sid, &soc_addr.un, addrlen);

  if (cc < OK)
    fprintf(stderr, "Connection failed %d\n", errno);
  else
#endif
  { for (ix = 0; ++ix <= 10; )
    { client_req('?', "whos there?");
    { Int len = strlen(&out_buf[sizeof(Int)]) + 2;

#if USE_CONTCP
      Cc cc = send(sid, &out_buf[sizeof(Int)-1], len, 0);
#else
      struct sockaddr * aa = &soc_addr.un;
      Cc cc = sendto(sid, &out_buf[sizeof(Int)-1], len, 0, 
                      aa, addrlen);
#endif
      if (cc < OK)
	fprintf(stderr, "Client send failed (%d) %d\n", errno, cc);
      else
      {
#if USE_CONTCP
        Int got = recv(sid, &in_buf[sizeof(Int)-1], sizeof(in_buf), 0);
#else
        int rlen = addrlen;
        Int got = recvfrom(sid, &in_buf[sizeof(Int)-1], sizeof(in_buf), 0,
                                 &soc_addr.un, &rlen
                          );
#endif            
	if (got < OK)
	{ fprintf(stderr, "C.Error %d reading sock %d\n", errno);
	}
	else
          client_reply(in_buf, got);
      }
      _sleep(1);
    }}
    client_req(0, "bye");
#if USE_CONTCP
    if (  send(sid, &out_buf[sizeof(Int)-1], 6, 0) < 6)
#else
    if (sendto(sid, &out_buf[sizeof(Int)-1], 6, 0, 
                 (struct sockaddr *)&soc_addr, addrlen)      < 6)
#endif
      0;
    close(sid);
  }
  return 0;
}}}}}




