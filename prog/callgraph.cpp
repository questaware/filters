
#include <string>
#include <vector>
#include <map>

#include <stdio.h>
#include <ctype.h>


using namespace std;

const vector<string> empty_vec;

struct Descr
{ int             callct;
  bool            printed;
  int		   				pyr_size;
  vector<string>  calls;
  
  Descr();
};

Descr::Descr() : callct(0), printed(0), pyr_size(0) {}

map<string, Descr *> dict;


int indent_ct;


void print_eny(string str)

{ int ix;
  for (ix = indent_ct; --ix >= 0; )
    printf("   ");

  printf("%s\n", str.c_str());

  map<string, Descr *>::iterator f = dict.find(str);

  if (f == dict.end() || f->second->printed)
    return;

  f->second->printed = 1;
 
//s->first.c_str());
  
  ++indent_ct;

  vector <string> & vv = f->second->calls;
  vector <string>::const_iterator v;
  string last;

  for (v = vv.begin(); v != vv.end(); ++v)
  { if (last != *v) 
      print_eny(*v);
    last = *v;
  }

  --indent_ct;
}


void explain()

{ printf("callgraph (no arguments)\n");
  exit(1);
}




int main (int argc, char * argv[])

{ char buf[133];
  char * ln;
  int rec_ct = 0;

  if (argc != 1)
     explain();

  while ((ln = fgets(buf, sizeof(buf)-1, stdin)) != NULL)
  { char * lnn;
    char * arg2 = "";

    for (lnn = ln-1; *++lnn != 0; )
      if (*lnn >= 'A' && *lnn <= 'Z') *lnn += 'a' - 'A';


    for (lnn = ln-1; *++lnn != 0 && !isspace(*lnn); )
      ;
    if (*lnn != 0)
    { *lnn = 0;
      for ( ; *++lnn != 0 && isspace(*lnn); )
        ;
      if (*lnn != 0)
      { if (strncmp(lnn, "calls ", 6) == 0)
          lnn += 6;
        arg2 = lnn;
        for ( ; *++lnn != 0 && !isspace(*lnn); )
          ;
				*lnn = 0;
      }
    }

    if (buf[0] == 'Z' && buf[1] == 'Z' && buf[2] == 'Z' && 
        strncmp(buf, "ZZZ Warning:", 12) == 0)
    { printf("%s", buf);
      continue;
    }

  //printf("one %s two %s\n", ln, arg2);
    ++rec_ct;

    map<string, Descr *>::iterator dcr = dict.find(ln);
    Descr * d;

    if (dcr != dict.end())
    { d = dcr->second;
    }
    else
    { d = new Descr();
      //printf("Add %s\n", ln);
      dict[ln] = d;
    }

    vector <string> & p = d->calls;

    p.push_back(arg2);
    
    dcr = dict.find(arg2);
    if (dcr != dict.end())
      d = dcr->second;
    else
    { d = new Descr();
      dict[arg2] = d;
      //printf("Addcalled %s\n", arg2);
    }
    d->callct += 1;
    dcr = dict.find(ln);
    if (dcr != dict.end())
    { if (dcr->second->calls.empty())
        printf("Called is empty!\n");
    /*else
        printf("Installed %s %d %s\n", dcr->first.c_str(), dcr->second->callct,
				       dcr->second->calls[0].c_str());*/
    }
  }

  map<string, Descr *>::iterator p;
  int iter;

  for (iter = rec_ct; --iter >= 0;)
  { 
    map<string, Descr *>::const_iterator top = dict.begin();
    int hval = 0;

    for (p = dict.begin(); p != dict.end(); ++p)
    { /*printf("Try %s %d %d %d\n", p->first.c_str(), 
		p->second->callct, p->second->printed, p->second->calls.size() );*/
      Descr * d = p->second;
      vector <string>::const_iterator v;
      for (v = d->calls.begin(); v != d->calls.end(); ++v)
      { Descr * cd = dict[*v];
        if (d->pyr_size <= cd->pyr_size)
          d->pyr_size = cd->pyr_size + 1;
      }
    }
  }

  //printf("Got Pyramid sizes\n");

  for (iter = rec_ct; --iter >= 0;)
  { 
    map<string, Descr *>::const_iterator top = dict.begin();
    int hval = -1;

    for (p = dict.begin(); p != dict.end(); ++p)
    { if (p->second->callct == 0 && !p->second->printed 
       && p->second->pyr_size > hval)
      { hval = p->second->pyr_size;
				top = p;
      }
    }

    if (hval < 0)
      break;
    //printf("Level %d\n", top->second->pyr_size);
    print_eny(top->first);

    printf("\n\n");
  }

  return 0;
}
