#include   <stdio.h>
#include   <conio.h>
#include   <windows.h>


typedef int Cc;


static void explain()

{ fputs(
  "clipboard val\n"
  "  where val is text\n", stdout
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

{ char * data = "";
  if (argc > 1)
  { data = argv[1];
  	if (data[0] == '-')
  		explain();
  	
 	{ int cc = ClipSet(data);
 	
  }}
}
