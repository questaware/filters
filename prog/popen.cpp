#include   <stdio.h>
#include	 <stdlib.h>
#include	 <stdarg.h>
#include   <conio.h>
#include   <windows.h>
#include   <process.h>
#include	 <sys/stat.h>

#if _MSC_VER < 1900
#define CONSTEXPR const
#else
#define CONSTEXPR constexpr
#endif

#if _DEBUG
#define NULL_OP stdout
#else
#define NULL_OP NULL
#endif


typedef short Cc;
#define OK 0
#define nullptr (0)

#define PATHCHR ';'
#define DIRSEPCHAR	'\\'

#define NFILEN 256

#define in_range(a,b,c) ((unsigned)((a)-(b)) <= (unsigned)((c)-(b)) && ((signed)c)-(b) >= 0)


char * concat(char * tgt, ...)
{
  va_list ap;
  va_start(ap, tgt);

{ char * s;
  tgt[0] = 0;

  while ((s = va_arg(ap, char *)) != nullptr)
    strcat(tgt, s);

  return tgt;
}}

					/* at most n chars including the 0 */

char * strpcpypfx(char * t_, const char * s_, int n_, char term)

{ short n;
  char ch;
  const char * s = s_;
  char * t = t_;

  for (n = -1; ++n < n_ && (ch = s[n]) != 0 && ch != term;
      )
    t[n] = ch;
  
  t[n] = 0;

  return t_;
}

					/* at most n chars including the 0 */
char * strpcpy(char * t_, const char * s_, int n_)
#if 0
{ register short n = n_;
  register char * s = s_;
  register char * t = t_;
 
  if (t != null)
  { while (--n > 0 && (*t++ = *s++) != 0)
      ;
  
    if (n == 0)
      *t = 0;
  }
  return t_;
}
#else
{ return strpcpypfx(t_, s_, n_-1, 0);
}
#endif

/* ################# string match routines ##################### */

char bad_strmatch;

const char * com_match(const char * t_, const char * s_, int mask)

{ register int tix = 0;
  register const char * s = s_;
  register char ch;

#if CHARCODE != ASCII
  if (mask == 0x20)
    for (; *s != 0 && (ch = t_[tix]) != 0 && toupper(ch) == toupper(*s); ++s)
      ++tix;
  else 
#endif
    for (; *s != 0 && (ch = t_[tix]) != 0 && (ch | mask) == (*s | mask); ++s)
      ++tix;
  bad_strmatch = *s;
  return &t_[tix];
}

/* replace the last entry in dir by file */
/* t can equal dir */

char * pathcat(char * t, int bufsz, const char * dir, const char * file)
	
{ register int tix;

  if (dir[0] == '.' && dir[1] == '/' && dir[2] == '.' && dir[3] == '.' && dir[4] == '/')
    dir += 2;

  if (file[0] == '/' || file[0] == '\\')
    strpcpy(t, file, bufsz);
  else
  { for (tix = -1; ++tix < bufsz - 1 && dir[tix] != 0; )
      t[tix] = dir[tix];

    t[tix] = 0;
   
    while (1)
    { while (--tix >= 0 && t[tix] != '/' && t[tix] != '\\')	/* strip back to / */
        ;

      if (tix > 0)
      { if (t[tix-1] != '.' &&
            file[0] == '.'  && file[1] == '.' && file[2] == '/')
        { for (file = &file[2]; *++file == '/';)		/* strip forward to non / */
            ;
          continue;
        }
      }

      break;
    }
  
  { int six = 0;
    for (; ++tix < bufsz - 1 && file[six] != 0; ++six)
      t[tix] = file[six];

    t[tix] = 0;
  }}
  return t;
}



int name_mode(const char * s)

{	struct stat fstat_;
	if (stat(s, &fstat_) != OK)
	  return 0;
	return fstat_.st_mode;
}

#define fexist(fn) name_mode(fn)


static char fspec[256+2];	/* full path spec to search */

bool fex_path(const char * dir, const char * file)
	
{ if (dir != nullptr)
  { char * pc = pathcat(&fspec[0], sizeof(fspec), dir, file);
    if (fexist(pc))
      return true;
  }
  
  return false;  
}

#define INTWIDTH (sizeof(int) * 3)


char * int_asc(int i)
			/* integer to translate to a string */
{
  static char result[INTWIDTH+2];
	memset(&result, ' ', INTWIDTH); 

{	char *sp = &result[INTWIDTH+1];
	int v = i;		/* sign of resulting number */

	if (v < 0)
	  v = -v;

	*sp = 0;
	do 
	{ *(--sp) = '0' + v % 10;	/* and install the new digit */
	  v = v / 10;
	} while (v);

	if (i < 0)
	  *(--sp) = '-';		/* and install the minus sign */

	return sp;
}}



char * g_invokenm;


void flook_init(char * cmd)

{ g_invokenm = _strdup(cmd);

  if (g_invokenm[0] == '"')
  { ++g_invokenm;
    g_invokenm[strlen(g_invokenm)-1] = 0;
  }
{ char * sp;
  for (sp = g_invokenm; *sp != 0 && *sp != ' '; ++sp)
    if (*sp == '\\')
      *sp = '/';

  if (*sp != 0)
    *sp = 0;

}}



/*	wh == I => 
	Look up the existence of a file in the current working directory || 
	along the include path in the INCLDIRS environment variable. 
	wh == E => 
	Look up the existence of a file in the current working directory || 
	along the normal $PATH
	else 
	  if wh is in uppercase then look first in the $HOME directory
	  always look up a file along the normal $PATH.
*/
const char * flook(char wh, const char * fname)

{          char uwh = toupper(wh);
  register char *path;	/* environmental PATH variable */
//         char buf[100];

	if (fexist(fname))
	  return fname;
			                  /* if we have an absolute path check only there! */
	if (*fname == '\\' || *fname == '/' ||
	    *fname != 0 && fname[1] == ':')
	  return nullptr;

	//loglog1("not so far %s", fname);

	if (fname != g_invokenm)
  { if (fex_path(flook('E', g_invokenm), fname))
	    return fspec;
	}

	path = getenv("PATH");

	if (path != nullptr)
	  for (; *path != 0; ++path)
	  { int ix;

	    if (path[0] == '-' && path[1] == 'I'
	     || path[0] == '.' && path[1] == '/')
	      path += 2;

/*	    fspec[0] = 0;
            if (inclmod != nullptr && path[0] != '/')
              strcpy(&fspec[0], inclmod);
*/
	    for (ix = -1; ix < (int)sizeof(fspec)-2 &&
	            	   *path != 0
          		   && *path != PATHCHR
		              ; )
	      fspec[++ix] = *path++;
	    
	    if (path[-1] != DIRSEPCHAR)
	      fspec[++ix] = '/';
	        
	    fspec[ix+1] = 0;	        
/*	{ char buf[200];
  	  strcat(strcat((buf, "PATHE "),fspec);
  	  MessageBox(nullptr, buf, "pe",MB_YESNO|MB_ICONQUESTION);
    } */
	    if (fex_path(fspec, fname))
	      return fspec;

	    if (*path == 0)
	      break;
	  }

	return nullptr;	/* no such luck */
}



char * mkTempName (/*out*/char *buf, const char *name)
{
#ifdef _CONVDIR_CHAR
 #define DIRY_CHAR _CONVDIR_CHAR
#else
 #define DIRY_CHAR DIRSEPCHAR
#endif
	static const char *tmpDir = NULL ;
				 char c2[2];
	const  char * td = tmpDir != NULL ? tmpDir : (char *)getenv("TEMP");

	c2[0] = c2[1] = 0;

	if (td == NULL)
#if (defined _DOS) || (defined _WIN32)
						/* the C drive : better than ./ as ./ could be on a CD-Rom etc */
		td = "c:\\" ;
#else
		td = "./" ;
#endif
	else
		if (td[strlen(td)-1] != DIRY_CHAR)
			c2[0] = DIRY_CHAR;
	
	tmpDir = td;
	concat(buf,td,c2,"me",int_asc(_getpid()),name,0);

	return &buf[strlen(buf)];
}


#define DUMMY_STDIN_FILE		 "stdin.~~~" 

HANDLE CreateF(char * filenm)

{ int gen = GENERIC_WRITE;
	int share = FILE_SHARE_WRITE;
	int cmd = CREATE_ALWAYS;
	HANDLE Hdl;
	int ct = 2;

	(void)mkTempName(filenm, DUMMY_STDIN_FILE);
	
	for (; ;)
	{	Hdl = CreateFile(filenm,gen,share,NULL,
											cmd,FILE_ATTRIBUTE_NORMAL,NULL);
		if (--ct == 0)
			break;
		if (Hdl >= 0) 					/* VALID_HANDLE_VALUE */
			CloseHandle(Hdl);

		gen = GENERIC_READ;
		share = FILE_SHARE_READ;
		cmd = OPEN_ALWAYS;
	}

	return Hdl;
}


bool v_opt = false;

void mbwrite(const char * msg)

{ if (v_opt)
		printf("%s\n", msg);
}



void flagerr(const char * fmt)

{ DWORD ec = GetLastError();
	fprintf(stderr, fmt, ec);
}


void ErrorMessage(const char *str)  //display detailed error info
{	LPVOID msg;
  FormatMessage(
              FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
              NULL,
              GetLastError(),
              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
              (LPTSTR) &msg,
              0,
              NULL
               );
	fprintf(stderr, "%s: %s\n",str, (const char*)msg);
  LocalFree(msg);
}


void setMyConsoleIP()

{ HANDLE ConsIn = GetStdHandle( STD_INPUT_HANDLE );
  if (ConsIn < 0)					                    /* INVALID_HANDLE_VALUE */
    flagerr("Piperr %d");

/* SetStdHandle( STD_INPUT_HANDLE, g_ConsIn ); */
  if (0 == SetConsoleMode(ConsIn, ENABLE_WINDOW_INPUT))
    flagerr("PipeC %d");
}


class Pipe
{ public:
	enum Modes {	WL_IHAND = 1,
								WL_SHELL = 2,
								WL_AWAIT_PROMPT = 4,
								WL_HOLD  = 8,
								WL_CNC   = CREATE_NEW_CONSOLE,
								WL_CNPG	 = CREATE_NEW_PROCESS_GROUP,
								WL_SPAWN   = 0x1000,
								WL_SHOWW	 = 0x2000,
								WL_NOIHAND = 0x4000 };

	typedef unsigned int CmdMode;

	static int menu[4];

#if _MSC_VER >= 1900
  static_assert(WL_CNC > WL_HOLD && WL_CNPG > WL_HOLD);
#endif

	static
	 Cc WinLaunch(Cc & sysRet, CmdMode flags,
							  const char *app = nullptr, const char * ca = nullptr, 
      			    const char * in_data = nullptr, 
      			    HANDLE ip = 0, const char *outfile = 0); 		// char *outErr,
};

Cc Pipe::WinLaunch(Cc & sysRet, CmdMode flags,
									 const char *app, const char * ca, 
				      	   const char * in_data, 
				      	   HANDLE ip, const char *outfile)	// char *outErr,
{ char buff[1024];           //i/o buffer
	const char * fapp = app == nullptr ? nullptr : flook('P', app);
	
	if      (fapp != nullptr)					// never use comspec
		app = fapp;
	else if (flags & WL_SHELL)
	{	const char * comSpecName = (char*)getenv("COMSPEC");
		if (comSpecName == nullptr)
			comSpecName = "cmd.exe";

		if (ca == nullptr)
		{ app = nullptr;
			ca = comSpecName;
		}
		else									/* Create the command line */
		{ int len = strlen(concat(buff," /c \"", app == nullptr ? "" : app,0));
			char * dd = buff+len;
			char ch;
			char prev = 'A';
			const char * ss = ca;
			if (len+strlen(ss) >= sizeof(buff))
				return -1;

			for (; (ch = *ss++); prev = ch)
			{	if (ch == '/' && 										// &&!(flags & LAUNCH_LEAVENAMES)
					  (in_range(toupper(prev), 'A','Z')
				  || in_range(prev, '0', '9')
					||					prev == '_'  || prev == ' '))
					ch = '\\';
		
				if (ch == '"')
					*dd++ = '\\';
				*dd++ = ch;
			}

			*dd = '"';
			dd[1] = 0;
			ca = buff;
			app = comSpecName;
		}
	}
	
	if (app != NULL && ca != NULL && ca[0] != ' ')
		return -1000;

{ int use_data = ip != 0 || in_data != nullptr;
	HANDLE newstdin = ip;								//pipe handles
  HANDLE newstdout = 0;
  HANDLE write_stdin = 0;
  HANDLE read_stdout = 0;
  STARTUPINFO si;
//SECURITY_DESCRIPTOR sd;               //security information for pipes
  PROCESS_INFORMATION pi;
  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = nullptr;
  sa.bInheritHandle = TRUE;         //allow inheritable handles

//memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	if (flags & WL_SHOWW)
	{ si.dwFlags |= STARTF_USESHOWWINDOW;
	  si.wShowWindow = SW_SHOWNORMAL;
	}

	if (flags & WL_SPAWN)
	{	if (CreatePipe(&read_stdout,&newstdout,&sa,0))  //create stdout pipe
			mbwrite("Created RSO");
		else
	  {	ErrorMessage("CreatePipe");
	   	return -1001;
	  }
	
		if (CreatePipe(&newstdin,&write_stdin,&sa,0))   //create stdin pipe
			mbwrite("Created WSO");
		else
		{	ErrorMessage("CreatePipe");
		 	CloseHandle(newstdout);
		 	CloseHandle(read_stdout);
	 	 	return -1000;
  	}

// 	GetStartupInfo(&si);      //set startupinfo for the spawned process
		si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	  si.wShowWindow = SW_HIDE;
	  si.hStdOutput = newstdout;
	  si.hStdError = newstdout;     //set the new handles for the child process
	  si.hStdInput = newstdin;
//  si.lpTitle = "Emsub";
#if 0
		if (si.hStdOutput != 0)
		{	HANDLE cur_proc = GetCurrentProcess();
								 
			BOOL s = DuplicateHandle(cur_proc,si.hStdOutput,
															 cur_proc,&si.hStdError,0,TRUE,
															 DUPLICATE_SAME_ACCESS) ;
		}
	  // mbwrite("IHAND");
#endif	  
	  flags |= WL_IHAND;
	}
	else
	{	if (si.hStdInput <= 0 && !use_data && (flags & WL_NOIHAND) == 0)
		{ char	dummyInFile[NFILEN] ; 						 /* Dummy input file */
			newstdin = CreateF(dummyInFile);
			si.hStdInput = newstdin;
		}
		if (si.hStdOutput <= 0 && outfile)
		{	
			newstdout = CreateFile(outfile,GENERIC_WRITE,FILE_SHARE_WRITE,&sa,
																CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,nullptr);
			si.hStdOutput = newstdout;
			if (si.hStdOutput < 0)
				fprintf(stderr,"CFOut Failed\n");
		}
		if (si.hStdInput > 0 || si.hStdOutput > 0)
			si.dwFlags |= STARTF_USESTDHANDLES;
	}

	if (v_opt)
  {	printf(app == nullptr ? "<no app>" : "%s\n", app);
		printf(ca == nullptr ? "<no args>" : "%s\n", ca);
		printf("%s\n", int_asc((int)si.hStdOutput));
	}
															  //spawn the child process
  if (!CreateProcess(app,
                     (char*)ca,
                     nullptr,nullptr,
										 (flags & WL_IHAND),
                		 flags & (WL_CNPG+WL_CNC),
                     nullptr,nullptr,&si,&pi))
  { ErrorMessage("CreateProcess");
    if (newstdin != 0) CloseHandle(newstdin);
    if (newstdout != 0) CloseHandle(newstdout);
    if (read_stdout != 0) CloseHandle(read_stdout);
    if (write_stdin != 0) CloseHandle(write_stdin);
    return -1002;
  }
  
	CloseHandle(pi.hThread);

{	DWORD exit = 0;  			//process exit code
  DWORD bread;  //bytes read
  DWORD avail;  //bytes available
  int got_ip = 0;
  int clamp = 4;
  int iter = 0;
  int end = 0;
	int sct = 6;
 	int sentz = 0;

	if (!(flags & WL_SPAWN))
	{
//  int ct = 200000;
		for (;;)
		{ DWORD procStatus = WaitForSingleObject(pi.hProcess, 200);
			if (procStatus == WAIT_TIMEOUT)
			{ // millisleep(100);
//			if (--ct >= 0)
//			if (!typahead() /* && (TTbreakFlag != 0)*/) 
//				continue;
			}
		{ Cc cc = (procStatus != WAIT_FAILED);
			if (cc)
			{ cc = GetExitCodeProcess(pi.hProcess,&exit);
			  if (cc == 0)
			  	flagerr("GECP %d");
				else
				{ // mbwrite("Exitting");
					break;
				}
			}
		}}
		mbwrite("Exit Whole");
	}
	else 
	{ char fbuff[512];
		char lbuff[512];
		const char * ipstr = in_data == nullptr ? "" : in_data;
  	FILE * op = outfile == nullptr ? NULL_OP : fopen(outfile, "w");
  	int append_nl = *ipstr != 0;
		
		if (read_stdout == 0)
		{ mbwrite("Int Err");
			return -1;
		}

	  for(;;++iter)      //main program loop
	  { Sleep(250);
	  { Cc cc = PeekNamedPipe(read_stdout,buff,1023,&bread,&avail,nullptr);
		  if      (cc == 0)
	  	 	flagerr("PNP %d");
	  	else
	  	{	//mbwrite(int_asc(avail+10000));
	                        //check to see if there is any data to read from stdout
		  	if (bread != 0)
	  		{ int done = 0;
	  			buff[bread] = 0;
//  			mbwrite(buff);
	    	  while (done < (int)avail)
	    	  {	cc = ReadFile(read_stdout,buff,1023,&bread,nullptr);
					  if (cc == 0)
	  				{	flagerr("PNP %d");
	    	 			break;
	    	 		}
	    	 		if (bread == 0)
	    	 			break;
	    		  done += bread;
	    		  buff[bread] = 0;
						got_ip = 1;
	  		    if      (op != nullptr)
	  		      fputs(buff, op);
	  		    else if (v_opt)
							printf("In: %s",buff);
	   			}
	   		}
			}
	  	cc = GetExitCodeProcess(pi.hProcess,&exit); //while the process is running
	    if (cc == 0)
	    {	flagerr("GECP %d");
	    	if (--clamp <= 0)
	    		break;
	    }
	    else if (exit != STILL_ACTIVE)
	    { mbwrite(int_asc(exit));
	      break;
	    }

			if (sentz && iter > 10)		// 4 seconds
			{ if (--sentz <= 0)
				{ sentz = 1;
					break;
				}
				continue;
			}

			mbwrite("Testing");

	    if ((flags & WL_AWAIT_PROMPT) && !got_ip && !_kbhit())
	      continue;

			mbwrite("Getting");

			if (*ipstr == 0)
	    {	if (ip != 0)
		    { unsigned long rct;
		    	cc = ReadFile(ip,fbuff,sizeof(fbuff)-1,&rct,nullptr);
					if (cc == 0)
						ip = 0;
					else
					{ ipstr = fbuff;
						fbuff[rct] = 0;
					}
				}
			}
		{	int sl = *ipstr;
			if (sl != 0)
	    { char * ln = lbuff-1;
	    	--ipstr;
	      while (*++ipstr != 0 && *ipstr != '\r' && *ipstr != '\n')
	        *++ln = *ipstr;

				if (*ipstr == '\r' && ipstr[1] == '\n')
					++ipstr;
	      if (*ipstr != 0)
	      	*++ln = *ipstr++;
	      else
	      {	*++ln = '\n';
	      	mbwrite("Ends");
	      }
	      sl = ln + 1 - lbuff;
		    if (sl > 1)
		   	{ if      (lbuff[sl-1] == 'Z' - '@')
		   			sentz = 8;
		    	else if (lbuff[sl-1] != '\n' && append_nl)
			    { lbuff[sl++] = '\n';
				   	append_nl = 0;
	  	  	}
				}
	    }
	    
	    if (sl == 0 && sentz == 0)
	    { sentz = 8;
	    	lbuff[sl++] = 'Z' -'@';
	      mbwrite("Z");
	    }
#if 0
	    --sct;
			if (sct >= 0)
		  {	char sch = lbuff[sl];
		    lbuff[sl] = 0;
		  	mlwrite("Sending %d %x %s",sl, lbuff[sl-1], lbuff);
				mbwrite(lastmesg);
		    lbuff[sl] = sch;
		  }
#endif
	    cc = WriteFile(write_stdin,lbuff,sl,&bread,nullptr); //send it to stdin
	    if (cc == 0)
		  	ErrorMessage("WriteFile");
//	  else
//	  {	mlwrite("Sent %d",bread);
//			mbwrite(lastmesg);
//		}		  

			lbuff[0] = 0;						// Conceal data
			lbuff[1] = 0;
			lbuff[2] = 0;
    }}}
		if (op != NULL_OP)
			fclose(op);
	}
  
//printf("Exitted %d\n", exit);
  
  CloseHandle(pi.hProcess);
  CloseHandle(newstdin);            //clean stuff up
  CloseHandle(newstdout);
  CloseHandle(read_stdout);
  CloseHandle(write_stdin);
	setMyConsoleIP();

  sysRet = sentz ? -1 : (Cc)exit;
  return sentz ? -1 : OK;
}}}


#define STANDALONE 1

#if STANDALONE

void explain(void)

{ fputs(
 "launch [-wh # | -f #] [-p app] [-a args] [-d data] [-i [file]] [-o file]\n"
 "    -wh #     Which command mode\n"
 "    -f  #     command mode\n"
 "    -p  app   App\n"
 "    -a  args  Args\n"
 "    -d  data  Data to pipe in\n"
 "    -o  file  File to receive output\n"
 "    -i [file] (Also)take data from stdin\n"
 "    -v        Verbose\n", stdout);
    exit(0);
}


int Pipe::menu[4]= { (int)(WL_SPAWN | WL_CNC | WL_AWAIT_PROMPT),
										 WL_SHELL | WL_CNPG | WL_NOIHAND,   //  | WL_SHOWW,
										 WL_SPAWN | WL_IHAND | WL_HOLD | WL_CNC,
										 WL_IHAND | WL_HOLD | WL_SHELL
									 };

int main(int argc, char **argv)

{ if (argc <= 1)
  { explain();
  }
  
{ int argsleft = argc - 1;
  char ** argv_ = &argv[1];

	short wh_opt = 0;
	int f_opt = 0;
	char * app = nullptr;
	char * args = nullptr;
	char * indata = nullptr;
	char * infile = nullptr;
	char * outfile = nullptr;
	bool usestdin_opt = false;
	
  for (; argsleft > 0 && argv_[0][0] == '-'; --argsleft)
  { char * flgs = &argv_[0][1]; 
    ++argv_;
  	if (*flgs != 0)
  	{	if      (*flgs == 'i')
  		{ if (argsleft == 1)
  				usestdin_opt = true;
  			else
  			{	infile = *(argv_++);
		  		--argsleft;
		  	}
  			continue;
  		}
		  else if (*flgs == 'v')
			{	v_opt = true;
  			continue;
  		}
			else
  		{	if (argsleft == 1)
  				explain();
  		}
  		switch(*flgs)
  		{ case 'w':	if (flgs[1] != 'h')
  									explain();
  								wh_opt = atoi(*(argv_));
  								break;
  			case 'f':	f_opt = atoi(*(argv_));
  								break;
  			case 'p':	app = *(argv_);
  								break;
  			case 'a':	args = *(argv_);
  								break;
  			case 'd':	indata = *(argv_);
  								break;
  			case 'o':	outfile = *(argv_);
  								break;
  			default:	explain();
  		}
  		++argv_;
  		--argsleft;
 		}
  }

	if (f_opt == 0 && wh_opt == 0)
		explain();
				
	if (wh_opt > sizeof(Pipe::menu) / sizeof(int))
		explain();

	if (wh_opt)
		if (f_opt)
			explain();
		else
			f_opt = Pipe::menu[wh_opt - 1];

	HANDLE ip = infile != nullptr
									? CreateFile(infile,GENERIC_READ,FILE_SHARE_READ,nullptr,
																OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr) :
							usestdin_opt ? GetStdHandle( STD_INPUT_HANDLE ) :
														0;
	Cc rc;
	Cc cc = Pipe::WinLaunch(rc, f_opt, app, args, indata, ip, outfile);
	if (cc == -1)
		printf("Gave up\n");
	else
		printf("Cc %d Rc %d\n", cc, rc);
	return OK;
}}

#endif

