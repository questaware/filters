#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

typedef int Cc;
typedef int Bool;
typedef unsigned int Set;
#define OK 0

#if 0
#if __unix__
#include <unistd.h>

typedef int Cc;
typedef int Bool;
typedef unsigned int Set;
#define OK 0

#else
#include "build.h"
#endif
#endif

extern char * match_re (char *, char *, Bool);

#ifdef _MSC_VER
#define PATHCHR ';'
#else
#define PATHCHR ':'
#endif

#define NFILEN 1000

#ifdef _MSC_VER

#include <windows.h>

void flagerr()  //display detailed error info

{	DWORD ec = GetLastError();
	LPVOID msg;
  FormatMessage(
              FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
              NULL,
              ec,
              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
              (LPTSTR) &msg,
              0,
              NULL
               );
  printf("detail: %d %s\n", ec, msg);
  LocalFree(msg);
}



int is_diry(const char * s)

{ char filen[NFILEN];
	char * t;
	strcpy(filen, s);
	
	for (t = filen-1; *++t != 0; )
		if (*t == '/')
			*t = '\\';

{	BY_HANDLE_FILE_INFORMATION fileinfo;
	int res = 0;
{ SECURITY_ATTRIBUTES sa;
  sa.lpSecurityDescriptor = NULL;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = FALSE;

{ HANDLE myfile = CreateFile(filen, 0, FILE_SHARE_READ, &sa,
														 OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (myfile == INVALID_HANDLE_VALUE)
	{ // flagerr();
		return 0;
	}
  
{	DWORD rc = GetFileInformationByHandle(myfile, &fileinfo);
	
	if (rc)
	{ res = fileinfo.dwFileAttributes
				& (FILE_ATTRIBUTE_DIRECTORY+FILE_ATTRIBUTE_ARCHIVE);	// Some archives are directories
	}
	
	CloseHandle(myfile);
	
	return res;
}}}}}

#else
int is_diry(const char * s)

{ struct stat stat_;
	if (stat(s, &stat_) != OK)
		retirn 0;
	return stat_.st_mode & _S_IFDIR;
}

#endif

void explain()

{ fputs(
 "edpath -= [-l] [-p dir]* [-a dir]* [-v pathname] [-r old:new] [path | - | ]\n"
 "   -l          -- Output entries one per line\n"
 "   -=          -- Output an assignment command\n"
 "   -p dir      -- Prepend this directory\n"
 "   -a dir      -- Append this directory\n"
 "   -v pathname -- Use this pathname  DEFAULT: PATH\n"
 "   -r old:new  -- Replace any text matching regular expression old with new\n"
 "               -- Does not modify -p, or -a text\n"
 "   -c          -- Remove directories that do not exist\n"
 "Take the path from path, stdin, or the contents of pathname.\n"
 "The resulting path with its prepends and appends, and with duplicates\n"
 "removed is printed on the stdout.\n"
 " Note: the usage eval `edpath -= -r old:new`\n", stderr);
}

void sho(char * txt)

{ fputs(txt, stdout);
	fputs("\n", stdout);
}


char * plist = NULL;
int 	 plist_len = 0;
int 	 plist_space = 0;


Cc add_entry(char * entry)

{ if (entry[0] == 0)
		entry = ".";

{ int len = strlen(entry);

	if (plist_len + len + 2 >= plist_space)
	{ char * pl = malloc(plist_space + 10002);
		if (pl == NULL)
			exit(7);
		if (plist != NULL)
		{ memcpy(pl, plist, plist_space);
			free(plist);
		}
		plist = pl;
		plist_space += 10000;
	}

	plist[plist_len] = PATHCHR;
	strcpy(&plist[plist_len+1], entry);
	plist_len += len + 1;
	/*printf("E %s\n", entry);*/

	return OK;
}}



Bool got_entry(char * entry)

{ char * s, * ss, * se;

	if (plist == NULL)
		return 0;
  
	if (entry[0] == 0)
		entry = ".";

	for (s = plist; *s == PATHCHR; )
	{ se = entry;
		/*printf("Try %s : %s\n", s+1, se);*/
		for (ss = s+1; *ss == *se && *se != 0; ++ss)
			++se;
		if ((*ss == PATHCHR || *ss == 0) && *se == 0)
		{/*printf("Got %s\n", entry);*/
			return 1;
		}
	  
		/*printf("NGot %s\n", entry);*/

		for ( ; *ss != PATHCHR && *ss != 0; ++ss)
			;
		s = ss;
	}

	return 0;
}



int main(int argc, char **argv)

{ if (argc <= 1)
	{ explain();
		exit(0);
	}
  
{ char buf[1000];
	int argsleft = argc - 1;
	char ** argv_ = &argv[1];
	char ** oldpat = calloc(argsleft, sizeof(char*));
	char ** newpat = calloc(argsleft, sizeof(char*));
	int reptop = -1;

	Set opts = 0;
	Bool eq_opt = 0;
	Bool c_opt = 0;
	Bool l_opt = 0;
	char * path = "PATH";
	char * pathsrc = NULL;

	char * begin_editable = NULL;
	char * end_editable = NULL;
  
	for (; argsleft > 0 && argv_[0][0] == '-' && argv_[0][1] != 0; --argsleft)
	{ char * flgs;
		for (flgs = &argv_[0][1]; *flgs != 0; ++flgs)
			if			(*flgs == 'p' && argsleft > 1)
			{ char * p_ = argv_[1];
				if (! got_entry(p_))
					add_entry(p_);
				--argsleft;
				++argv_;
				break;
			}
			else if (*flgs == '=')
			{ eq_opt = 1;
			}
			else if (*flgs == 'l')
			{ l_opt = 1;
			}
			else if (*flgs == 'c')
			{ c_opt = 1;
			}
			else if (*flgs == 'a' && argsleft > 1)
			{ --argsleft;
				++argv_;
				break;
			}
			else if (*flgs == 'r' && argsleft > 1)
			{ char * p_ = argv_[1];
				char * p__ = strdup(p_);
				while (*p_ != 0 && *p_ != ':')
					++p_;

				if (*p_ == 0)
					explain();
				else
					p__[p_ - argv_[1]] = 0;
			  
				oldpat[++reptop] = p__;
				newpat[reptop] = p_ + 1;
				--argsleft;
				++argv_;
				break;
			}
			else if (*flgs == 'v' && argsleft > 1)
			{ path = argv_[1];
				--argsleft;
				++argv_;
				break;
			}
			else
			{ explain();
				exit(0);
			}
		++argv_;
	}

	if (l_opt)
		eq_opt = 0;

	begin_editable = plist == NULL ? NULL : &plist[strlen(plist)];

	switch (0)
	{ default:
		if (argsleft > 0)
		{ char * src = argv_[0];
			if (src[0] != '-' || src[1] != 0)
				pathsrc = src;
			else
			{ char * ln, * ln_;
	 
				while (1)
				{ ln = fgets(&buf[0], sizeof(buf), stdin);
					if (ln == NULL)
						break;
					ln[strlen(ln)-1] = 0;
					 
					for (ln_ = ln; ; ++ln_)
					{ if (*ln_ == PATHCHR || *ln_ == 0)
						{ Bool end = *ln_ == 0;
							*ln_ = 0;
							if (! got_entry(ln))
								add_entry(ln);
							if (end)
								break;
							ln = ln_ + 1;
						}
					}
				}
				break;
			}
		}
		if (pathsrc == NULL)
		{ pathsrc = getenv(path);
			if (pathsrc == NULL)
			{ printf("Environment variable not defined %s\n", path);
				exit(6);
			}
		}
		pathsrc = strdup(pathsrc);
	{ char * s, *ss;
		int clamp = 200;
		Bool end;
		for (s = pathsrc-1; ; )
		{ for ( ss = s; *++ss != PATHCHR && *ss != 0; )
				;
			end = *ss == 0;
			*ss = 0;

			if (! got_entry(s+1))
				add_entry(s+1);
		  
			if (end)
				break;
			if (--clamp <= 0)
				break;
			*ss = PATHCHR;
			s = ss;
			if (*s == 0)
				break;
		}
		break; 	 
	}}

	end_editable = plist == NULL ? NULL : &plist[strlen(plist)];

	argsleft = argc - 1;
	argv_ = &argv[1];
  
	for (; argsleft > 0 && argv_[0][0] == '-' && argv_[0][1] != 0; --argsleft)
	{ char * flgs;
		for (flgs = &argv_[0][1]; *flgs != 0; ++flgs)
			if			(*flgs == 'a' && argsleft > 1)
			{ char * p_ = argv_[1];
				if (! got_entry(p_))
					add_entry(p_);
				--argsleft;
				++argv_;
				break;
			}
			else if (*flgs == '=' || *flgs == 'l')
				;
			else if ((*flgs == 'p' || *flgs == 'v' || *flgs == 'r') && argsleft > 1)
			{ --argsleft;
				++argv_;
				break;
			}
		++argv_;
	}

	if (begin_editable == NULL)
		begin_editable = plist;

	if (end_editable == NULL)
		end_editable = plist;

	if (eq_opt)
		printf("%s=", path);

{ char * s, *sahead, *sss;
	Bool end;
	int clamp = 200;
	char buf[NFILEN+1];

	/*printf("PLL %s\n", plist);*/

	for (sahead = plist+1; *sahead != 0; )
	{ s = sahead;
		while (*++sahead != PATHCHR && *sahead != 0)
			;
			
		++sahead;
		if (sahead - s > sizeof(buf) - 1)
			continue;

		end = sahead[-1] == 0;
		sahead[-1] = 0;

		strcpy(buf, s);

		if (s < begin_editable || s >= end_editable)
			;
		else
		{ --s;

			while (*++s != 0)
			{ int rix;
				for (rix = reptop+1; --rix >= 0; )
				{ char * rep = oldpat[rix];
					for (sss = s-1; *++sss != 0; ) 
					{ /*printf("TRY %s ::: %s\n", rep, sss);*/
					{ char * mm = match_re(sss, rep, 0);
						if (mm != sss)
						{ /*printf("GOT %s\n", mm);*/
							memcpy(buf, s, sss - s);
							strcpy(sss, newpat[rix]);
							break;
						}
					}}
					if (*sss != 0)
						break;
				}
			}
		}

	{ int missing = c_opt;
		if (missing && (buf[0] == '/' || buf[1] == ':' && PATHCHR != ':'))
		{ if (is_diry(buf))
				missing = 0;
		}

		if (!missing)
			if (l_opt)
				printf("%s\n", buf);
			else
				fputs(buf, stdout);

		if (end)
			break;
		if (--clamp <= 0)
			break;

		if (!l_opt && !missing)
			fputc(PATHCHR, stdout);
	}}

	return 0;
}}}
