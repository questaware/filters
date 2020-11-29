#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <string>
#include <vector>
#include <map>

#define DBG 1

#define true 1
#define false 0

using namespace std;

const vector<string> empty_vec;


map<string, string> dict;

#define strnicmp strncasecmp



void explain()

{ 
  printf("lbltofile { -t tagfile }  { file }\n"
        );
  exit(1);
}



int main (int argc, char * argv[])

{ 
  char buf[133];
  char * ln;
  int rec_ct = 0;

  char * fn = NULL;
  char * tagf = "tags";

  int argsleft = argc;
  char ** argv_ = &argv[1];

  for (; --argsleft > 0; ++argv_)
  { if (argv_[0][0] == '-')
    { if      (argv_[0][1] == 't' && argsleft > 1)
        tagf = argv_[1];
      else
        explain();
      --argsleft;
      ++argv_;
    }
    else
    { if (fn != NULL)
        explain();
      fn = argv[1];
    }
  }
  
  if (fn == NULL)
    fn = "-";

{ FILE * tip = fopen(tagf, "r");
  if (tip == NULL)
  { printf("Cannot open tag file %s\n", tagf);
    explain();
  }
  
  while ((ln = fgets(buf, sizeof(buf)-1, tip)) != NULL)
  { char * lnn;
    char * we;
    char * pth;
    char * pe;
    for (lnn = ln-1; *++lnn != 0 && isspace(*lnn); )
      ;
    for (we = lnn-1; *++we != 0 && !isspace(*we); )
      ;

    if (*we == 0) continue;

    *we = 0;

    for (pth = we; *++pth != 0 && isspace(*pth); )
      ;
    for (pe = pth-1; *++pe != 0 && !isspace(*pe); )
      ;
    *pe = 0;
    if (*pth != '=')
      dict[lnn] = pth;
  }

{ FILE * ip  = fn[0] == '-' && fn[1] == 0 ? stdin : fopen(fn, "r");
  if (ip == NULL)
  { printf("Cannot open file %s\n", fn);
    explain();
  }

  while ((ln = fgets(buf, sizeof(buf)-1, ip)) != NULL)
  { char * lnn;
    char * le;
#if DBG > 9
    printf("Read:%s", buf);
#endif
    for (lnn = ln-1; *++lnn != 0 && isspace(*lnn); )
      ;
    for (le = lnn-1; *++le != 0 && !isspace(*le); )
      ;

    *le = 0;
    if (*lnn == 0) continue;
    
  { map<string, string>::iterator dcr = dict.find(lnn);
    if (dcr == dict.end())
    { int ix;
      for (ix = -1; lnn[++ix] != 0; )
        lnn[ix] = tolower(lnn[ix]);
      dcr = dict.find(lnn);
    }
    if (dcr == dict.end())
    { int ix;
      for (ix = -1; lnn[++ix] != 0; )
        lnn[ix] = toupper(lnn[ix]);
      dcr = dict.find(lnn);
    }
    if (dcr != dict.end())
    { if (dcr->second != "")
        printf("%s\n", dcr->second.c_str());
      dcr->second = "";
    }
    else
    { 
#if DBG
       printf("Could not find %s\n", lnn);
       dict[lnn] = "";
#endif
    }
  }}
  return 0;
}}}

