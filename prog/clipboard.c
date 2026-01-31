#include   <stdio.h>
#include   <stdlib.h>
#include   <conio.h>
#include   <windows.h>


typedef int Cc;


static void explain()

{ fputs(
  "clipboard {-x x} {-y y} {-s s} val+\n"
  "  where\n"
  "      -x x gives the x coordinate of the shortcut\n"
  "      -y y gives the y coordinate of the shortcut\n"
  "      -s y gives the step of the click points\n"
  "      val is text\n", stdout
          );
  exit(0);
}


static HANDLE	g_hClipData;

static int g_clix = 0;


static
Cc OpenClip()

{	HWND mwh = GetTopWindow(NULL);
	if (mwh == NULL)
		return -1;

	return OpenClipboard(mwh);
}


Cc ClipSet(char const * data)

{ Cc cc = OpenClip();
	if (cc > 0)
	{	
		EmptyClipboard();
	{	int len = strlen(data);
		HANDLE m_hData = GlobalAlloc(GMEM_DDESHARE, len + 20 + 10);
		if (!m_hData)  
			return -1;
	
	{	char * m_lpData = (char*)GlobalLock(m_hData);
		if (m_lpData == NULL)
			return -1;
	
		strcpy(&m_lpData[0], data);
		SetClipboardData(CF_TEXT, m_hData);
		GlobalUnlock(m_hData);
	}}}

	CloseClipboard();
	
	return 0;
}





int main (int argc, char**argv)

{ char * data[100];
	int i;
	for (i = sizeof(data) / sizeof(data[0]); --i >= 0;)
		data[i] = "";

{ int xcoord = 0;
	int ycoord = 5;
	int ystep = 30;
	int	v_opt = 0;

	int argsleft = argc;
  char ** argv_ = &argv[0];
  
  for ( ; ; )
  { ++argv_;
    
		if (--argsleft <= 0)
			break;

    if (argv_[0][0] == '-')  
    {	if      (argv_[0][1] == 'v')
      { v_opt = 1;
      	continue;
      }
      else if (argsleft > 0)
      {	if (argv_[0][1] == 'x')
      	{ xcoord = atoi(argv_[1]);
        	continue;
        }
      	else if (argv_[0][1] == 'y')
      	{ ycoord = atoi(argv_[1]);
        	continue;
        }
      	else if (argv_[0][1] == 's')
      	{ ystep = atoi(argv_[1]);
					if (ystep > 0)
        		continue;
        }
      }
    }
		else if (i < 99)
    {	data[++i] = argv_[0];
    	if (v_opt)
				printf("AC %d D %s\n", argc, data[i]);
			continue;
    }

   	explain();
   	break;
  }
  
  if (argc > 1)
  { 
  	POINT p;

    if (GetCursorPos(&p))
    {	int cabs = p.y - ycoord;
    	int cix = cabs / ystep;
    	if (cix < 0 || cix > i)
			{	printf("OOr %d %d (%d)\n", cix, i, p.y);
				v_opt = 1;
			}
			else
		 	{ int cc = ClipSet(data[cix]);
		 		if (v_opt)
		 			printf("Click %d Cix %d (%d)\n", p.y, cix, ystep);
		 	}
    }
    else
    { printf("Could not get cursor\n");
			v_opt = 1;
    }
  }

	if (v_opt)
	{ fputs("Press return\n", stdout);
		getc(stdin);
	}
}}

