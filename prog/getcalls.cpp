#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <string>
#include <vector>
#include <map>

#define DBG 0

#define true 1
#define false 0

using namespace std;

const vector<string> empty_vec;


map<string, int> dict;
map<string, int> locvardict;

typedef struct Filenm
{ struct Filenm* next;
  char name[132];
} Filenm;

char * macro = "???";
int    macrolen = 3;
int    macrotyp = 0;

int expnstack[100];


#ifndef WIN32

#define strnicmp strncasecmp
#define stricmp strcasecmp

#endif

#if 0

int strnicmp(const char * s, const char * t, int n)

{ int ct = n;
  while (*s != 0 && toupper(*s) == toupper(*t) && ct > 0)
  { --ct;
    ++s;
    ++t;
  }
  return ct;
}

#endif


char * getword(const char * s)

{ static char buf[100];
  int ix;
  int clamp = sizeof(buf)-1;

  for (ix = -1; (isalnum(*s) || *s == '_') && --clamp > 0; ++s)
    buf[++ix] = *s;

  buf[++ix] = 0;   
  return buf;
}



void explain()

{ 
  printf("getcalls {-d macro{=#}} {-f fnlist}\n"
         "  types of macro:\n"
         "    1 convert to upper case\n"
         "    2 convert to lower case\n"
         "  -f fnlist  -- List of files, default stdin\n"
         "  -g         -- generic\n"
        );
  exit(1);
}



int main (int argc, char * argv[])

{ Filenm * filelist;
  Filenm * filetail = NULL;
  char buf[133];
  char wbuf[133];
  char wbuf2[133];
  char * ln;
  int rec_ct = 0;

  char * fn = "-";

  int argsleft = argc;
  char ** argv_ = &argv[1];

  for (; --argsleft > 0; ++argv_)
  { if (argv_[0][0] == '-')
    { if      (argv_[0][1] == 'd' && argsleft > 1)
      { char * e;
        for (e = argv_[1]-1; *++e != 0 && *++e != '='; )
          ;
        if (*e != 0)
        { macrotyp = atoi(e+1);
          *e = 0;
        }
        macrolen = strlen(argv_[1])+1;
        macro = (char*)malloc(macrolen);
        strcpy(macro, argv_[1]);
        strcat(macro,"(");
#if DBG > 1
        printf("Macro %d:%s\n", macrolen, macro);
#endif
      }
      else if (argv_[0][1] == 'f' && argsleft > 1)
        fn = argv_[1];
      else
        explain();
      --argsleft;
      ++argv_;
    }
    else
      explain();
  }

{ FILE * ip  = fn[0] == '-' && fn[1] == 0 ? stdin : fopen(fn, "r");
  if (ip == NULL)
  { printf("Cannot open file %s\n", fn);
    explain();
  }

  while ((ln = fgets(buf, sizeof(buf)-1, ip)) != NULL)
  { char * lnn;
    char * dot;
    for (lnn = ln-1; *++lnn != 0 && !isspace(*lnn); )
      ;
    *lnn = 0;
    for (dot = buf; *++dot != 0 && *dot != '.'; )
      ;
    wbuf[0] = 0;

  { int  cmtstate = 0;
    int  pnest = 0;
    int  gotp = 0;
    char lastword[133];
    char ftype = dot[0] == 0            ? 'C' : 
                 toupper(dot[1]) == 'F' ? 'F' : 'C';
    int  nmlen = strlen(buf);
    Filenm * cell = (Filenm*)malloc(sizeof(Filenm)+nmlen+2-sizeof(cell->name));
    FILE * fip = fopen(buf, "r");
    if (fip == NULL)
      printf("Cannot open %s\n", buf);
    else
    { strcpy(cell->name, buf);
      cell->next = 0;
      if (filetail == NULL)
      { filetail = cell;
        filelist = cell;
      }
      else
      { filetail->next = cell;
        filetail = cell;
      }
#if DBG > 1
      printf("file:%c %s\n", ftype, buf);
#endif
      lastword[0] = 0;

      while ((ln = fgets(buf, sizeof(buf)-1, fip)) != NULL)
      { char * lnn;
        int wordix = -1;

        if (ftype == 'F' && toupper(buf[0]) == 'C') continue;

        while (*ln != 0 && cmtstate >= 0)
        { ++wordix;
          for (lnn = ln-1; *++lnn != 0 && isspace(*lnn); )
            ;
          if (*lnn == 0)
            break;

        { char * le;
          if (isalnum(*lnn) || *lnn == '_') 
          { for (le = lnn; *++le != 0 && (isalnum(*le) || *le == '_'); )
              ;
            if (ftype == 'F' && wordix == 0 && 
                 strnicmp(lnn,"INTERFACE",9)== 0) break;
               
            if (ftype == 'F' && wordix == 0 &&
                (strnicmp(lnn,"INTEGER",7)==0   ||strnicmp(lnn,"LOGICAL",7)==0 ||
                 strnicmp(lnn,"CHARACTER",9)== 0||strnicmp(lnn,"REAL",4)==0    ||
                 strnicmp(lnn,"SUBROUTINE",10)== 0||strnicmp(lnn,"TYPE",4)==0
                ))
            { char ch;
              if (*le == '*' && isdigit(le[1]))
                while ((ch = *++le) && isdigit(ch))
                  ;
              for (le = le-1; *++le != 0 && isspace(*le); )
                ;
              
              if (strnicmp(lnn,"SUBROUTINE",10)== 0)
              { dict[getword(le)] = 1;
#if DBG
                printf("GOT.FS %s\n", getword(le));
#if DBG > 1
                printf("L:%s", buf);
#endif
#endif
              }
              else if (!(isalnum(le[8])|| le[8]=='_') && 
                       strnicmp(le, "FUNCTION", 8) == 0)
              { for (le = le+8; *++le != 0 && isspace(*le); )
                  ;
                dict[getword(le)] = 1;
#if DBG
                printf("GOT.FF %s\n", getword(le));
#if DBG > 1
                printf("L:%s", buf);
#endif
#endif
              }
              else
              { while (1)
                { string wd = getword(le);
                  locvardict[wd] = 1;
#if DBG
                  printf("GOT.LV %s\n", getword(le));
#endif
                  for (; *++le=='_'||isalnum(*le)||isdigit(*le); )
                   ;
                  for (le = le-1; *++le != 0 && isspace(*le); )
                   ;
                  if (*le == '(')
                  { locvardict[wd] = 2;

                    for (; *++le != 0 && *le != ')'; )
                      ;
                    if (*le == 0) break;
                    continue;
                  }
                  if (*le != ',') break;
                  for (; *++le != 0 && isspace(*le); )
                   ;
                  if (*le != ',') break;
                }
              }
            }
            if (cmtstate == 0)
            { /*printf("LW:%s", lnn);*/
              strcpy(lastword,lnn);
              if (strnicmp(lnn,macro, macrolen) == 0)
              { char * lnnn = lnn + macrolen;
                for (le = lnnn; *++le != 0 && (isalnum(*le) || *le == '_'); )
                  ;
                if (*le == ')')
                { int ix;
                  strcpy(lastword,lnnn);
                  switch (macrotyp)
                  { case 1:
                       for (ix = -1; lastword[++ix] != 0; )
                          lastword[ix] = toupper(lastword[ix]);
                       break;
                    case 2:
                       for (ix = -1; lastword[++ix] != 0; )
                          lastword[ix] = tolower(lastword[ix]);
                  }
#if DBG > 1
                  printf("LWW:%s", lastword);
#endif
                  ++le;
                }
              }
            }
            ln = le;
          }
          else if (ftype=='F' && lnn[0]=='!' || 
                   ftype=='C' && lnn[0]=='/' && lnn[1]=='/' && cmtstate==0)
          { cmtstate = -1;
            break;
          }
          else 
          {/*printf("punc %d:%30s X %20s", cmtstate, lnn, lastword);*/
            if (cmtstate >= 0)
            { if (lnn[0] == ';')
                wbuf[0] = 0;
              if (lnn[0] == ',' && pnest == 0)
                wbuf[0] = 0;
              if (gotp && lnn[0] == '{' && ftype == 'C')
              { if (wbuf[0] != 0) 
                { dict[getword(wbuf)] = 1;
#if DBG
                  printf("GOT.C %s\n", getword(wbuf));
#endif
                }
                wbuf[0] = 0;
              }
              if (lnn[0]=='(' && lastword[0]!=0 && pnest == 0 &&
                  (tolower(lastword[0]) != 'i' || 
                   tolower(lastword[1]) != 'f' || isalnum(lastword[2])) &&
                  (tolower(lastword[0]) != 'w' || 
                   tolower(lastword[1]) != 'h' ||
                   tolower(lastword[2]) != 'i' || 
                   tolower(lastword[3]) != 'l' ||
                   tolower(lastword[4]) != 'e' || isalnum(lastword[5])) &&
                  (strncmp(lastword, "switch",6) || isalnum(lastword[6]))
                  && cmtstate==0)
              { strcpy(wbuf, lastword);
              /*printf("Buff:%s", lastword);*/
              }
              if (lnn[0] == '(')
              { if (pnest == 0) 
                  gotp = 0;
                ++pnest;
              }
              if (lnn[0] == ')')
              { --pnest;
                gotp = pnest == 0;
              }
              else if (lnn[0] != '/' && lnn[1] != '*')
                gotp = 0;
            }

            le = lnn;
            if (ftype=='C')
            { if (cmtstate == 0 && le[0] == '/' && le[1] == '*')
              { cmtstate = 1;
                le += 1;
              }
              if (cmtstate > 0 &&  le[0] == '*' && le[1] == '/')
              { cmtstate = 0;
                le += 1;
              }
            }

            ln = le+1;
            lastword[0] = 0;
          }
        }}
        if (cmtstate < 0)
          cmtstate = 0;
      }    
      fclose(fip);
    }
  }}
  
  fclose(ip);

  for (; filelist != NULL; filelist = filelist->next)
  { char * ln = filelist->name;
    char * lnn;
    char * dot;
    for (lnn = ln-1; *++lnn != 0 && !isspace(*lnn); )
      ;
    *lnn = 0;
    for (dot = ln; *++dot != 0 && *dot != '.'; )
      ;
    wbuf[0] = 0;
    wbuf2[0] = 0;
  { int cmtstate = 0;
    int  pnest = 0;
    int  gotp = 0;
    int inhibit = 0;
    char lastword[133];
    char ftype = dot[0] == 0 ? 0 : 
                 dot[1] == 'f' ||  dot[1] == 'F' ? 'F' : 'C';
    FILE * fip = fopen(ln, "r");
    if (fip == NULL)
      printf("Cannot open %s\n", ln);
    else
    { 
#if DBG > -1
      printf("File%c %s\n", ftype, ln);
#endif
      lastword[0] = 0;

      while ((ln = fgets(buf, sizeof(buf)-1, fip)) != NULL)
      { char * lnn;
        int wordix = 0;

        if (ftype == 'F' && toupper(buf[0]) == 'C') continue;

        memset(expnstack,0,sizeof(expnstack));

        while (*ln != 0 && cmtstate >= 0)
        { for (lnn = ln-1; *++lnn != 0 && isspace(*lnn); )
            ;
          if (*lnn == 0)
            break;

        { char * le;
          if (isalnum(*lnn) || *lnn == '_') 
          { for (le = lnn; *++le != 0 && (isalnum(*le) || *le == '_'); )
              ;
            if (ftype == 'F' && wordix == 0 && 
                 strnicmp(lnn,"INTERFACE",9)== 0) break;
               
            if (ftype == 'F' && wordix == 0 &&
                (strnicmp(lnn,"INTEGER",7)==0   ||strnicmp(lnn,"LOGICAL",7)==0 ||
                 strnicmp(lnn,"CHARACTER",9)== 0||strnicmp(lnn,"REAL",4)==0    ||
                 strnicmp(lnn,"SUBROUTINE",10)== 0 || 
                 strnicmp(lnn,"PROGRAM",7)== 0 
                ))
            { char ch;
              if (*le == '*' && isdigit(le[1]))
                while ((ch = *++le) && isdigit(ch))
                  ;
              for (le = le-1; *++le != 0 && isspace(*le); )
                ;
              
              if      (!strnicmp(lnn,"SUBROUTINE",10)||!strnicmp(lnn,"PROGRAM",7))
              { char * g = getword(le);
                if (*g != 0) 
                { strcpy(wbuf2, g); 
#if DBG > 1
                  printf("GOTT %s\n", getword(le));
                  printf("Line:%s\n", buf);
                  printf("Word:%s\n",lnn);
                  printf("LV.0\n");
#endif
                  locvardict.clear();
                }
              }

              else if (!(isalnum(le[8]) || le[8] == '_') && 
                  strnicmp(le, "FUNCTION", 8) == 0)
              { for (le = le+8; *++le != 0 && isspace(*le); )
                  ;
                strcpy(wbuf2, getword(le));
#if DBG > 1
                printf("gott %s\n", getword(le));
                printf("LV.0\n");
#endif
                locvardict.clear();
              }
              else
              { string wd = getword(le);
                locvardict[wd] = 1;
#if DBG
                printf("GOT.LV %s\n", getword(le));
#endif
                for (; *++le=='_'||isalnum(*le)||isdigit(*le); )
                 ;
                for (le = le-1; *++le != 0 && isspace(*le); )
                 ;
                if (*le == '(')
                { locvardict[wd] = 2;
#if DBG
                  printf("GOT.TWO %s\n", wd.c_str());
#endif
                }
              }
            }
            if (ftype == 'F' && wordix == 0 &&
                (strnicmp(lnn,"INTEGER",7)==0   ||strnicmp(lnn,"LOGICAL",7)==0 ||
                 strnicmp(lnn,"CHARACTER",9)== 0||strnicmp(lnn,"REAL",4)==0    ||
                 strnicmp(lnn,"SUBROUTINE",10)== 0||strnicmp(lnn,"RECORD",6)==0
                ))
               inhibit = 1;

            if (ftype == 'F' && 
                !(isalnum(lastword[4]) || lastword[4] == '_') && 
                strnicmp(lastword, "CALL",4)==0)
            { char * t = getword(lnn);
              if (ftype == 'F' && *t > ' ')
              { int ix;
                for (ix = -1; t[++ix] != 0; )
                  if (t[ix] >= 'a')
                     t[ix] -= 'a' - 'A';
                for (ix = -1; wbuf2[++ix] != 0; )
                  if (wbuf2[ix] >= 'a')
                     wbuf2[ix] -= 'a' - 'A';
              }
              printf("%s CALLS %s\n", wbuf2, t);
              lastword[0] = '?';
            } 
            else if (cmtstate == 0)
            {/*printf("LW %d:%s", pnest,lnn);*/
              strcpy(lastword, lnn);
              if (strnicmp(lnn,macro, macrolen) == 0)
              { char * lnnn = lnn + macrolen;
                for (le = lnnn; *++le != 0 && (isalnum(*le) || *le == '_'); )
                  ;
                if (*le == ')')
                { int ix;
                  strcpy(lastword,lnnn);
                  switch (macrotyp)
                  { case 1:
                       for (ix = -1; lastword[++ix] != 0; )
                          lastword[ix] = toupper(lastword[ix]);
                       break;
                    case 2:
                       for (ix = -1; lastword[++ix] != 0; )
                          lastword[ix] = tolower(lastword[ix]);
                  }
#if DBG > 1
                  printf("LWW:%s", lastword);
#endif
                  ++le;
                }
              }
            }
            ln = le;
          }
          else if (ftype=='F' && lnn[0]=='!' || 
                   ftype=='C' && lnn[0]=='/' && lnn[1]=='/' && cmtstate==0)
          { cmtstate = -1;
            break;
          }
          else 
          { if (gotp && cmtstate == 0 && ftype == 'C')
            {  if (lnn[0] == '{')
               { if (wbuf[0] != 0)
                 { strcpy(wbuf2,getword(wbuf));
#if DBG > 1
                   printf("GOTT %s\n", getword(wbuf2));
#endif
                 }
                 wbuf[0] = 0;
               }
               else
               { char * s = getword(wbuf);
                 map<string, int>::iterator dcr = dict.find(s);
                 if (dcr != dict.end() && wbuf2[0] != 0 && stricmp(wbuf2,s) && *s > ' ')
/*               if (strcmp(s, "if") && strcmp(s,"while") && strcmp(s,"return"))*/
                   printf("%s CALLS %s\n", wbuf2, s);
               }
            }

            if (cmtstate == 0)
            {  if (lnn[0] == ';')
                 wbuf[0] = 0;
               if (lnn[0] == ',' && pnest == 0)
                 wbuf[0] = 0;
              if (lnn[0] == '(' && lastword[0] != 0 && cmtstate==0 && pnest>=0 &&
                  (tolower(lastword[0]) != 'i' || 
                   tolower(lastword[1]) != 'f' || isalnum(lastword[2])) &&
                  (tolower(lastword[0]) != 'w' || 
                   tolower(lastword[1]) != 'h' ||
                   tolower(lastword[2]) != 'i' || 
                   tolower(lastword[3]) != 'l' ||
                   tolower(lastword[4]) != 'e' || isalnum(lastword[5])) &&
                  (strncmp(lastword, "switch",6) || isalnum(lastword[6])))
              {/*printf("Consider %s", lastword);*/
                strcpy(wbuf, lastword);
                expnstack[pnest] = 1;
                if (ftype == 'F' && !inhibit)
                { char * s = getword(wbuf);
                  int lnest = 0;
                  char * ss;
                  for (ss = lnn; *++ss != 0 && lnest >= 0; )
                  { if (*ss == '(')
                      ++lnest;
                    if (*ss == ')')
                      --lnest;
                    if (lnest == 0 && *ss == ':')
                      break;
                  }
                  if (*ss == 0 || lnest < 0)
                  { 
                    map<string, int>::iterator dcr = dict.find(s);
                    if (dcr != dict.end() && wbuf2[0] != 0 && stricmp(wbuf2,s) && *s > ' ')
/*                  if (stricmp(s, "if") && stricmp(s,"while") && 
                        stricmp(s,"return") && stricmp(s,"parameter") &&
                        stricmp(wbuf2,s))*/
                      printf("%s CALLS %s\n", wbuf2, s);
                    map<string, int>::iterator dc = locvardict.find(s);
                    if (*s != 0 && strnicmp(s,"CASE",4))
                    if (dc != locvardict.end() &&
                        dc->second == 1 && wbuf2[0] != 0 && stricmp(wbuf2,s))
/*                  if (strcmp(s, "if") && strcmp(s,"while") && strcmp(s,"return"))*/
                      printf("%s CALLS %s\n", wbuf2, s);
#if DBG > 1
                    else
                      printf("%s NCALLS %s\n", wbuf2, s);
#endif
                  }
                }
              }
             
              if (lnn[0] == '(')
                ++pnest;
              if (lnn[0] == ')')
              { --pnest;
                gotp = expnstack[pnest] > 0;
                expnstack[pnest] = 0;
              }
              else if (lnn[0] != '/' && lnn[1] != '*')
                gotp = 0;
            }

            le = lnn;
            if (ftype=='C')
            { if (cmtstate == 0 && le[0] == '/' && le[1] == '*')
              { cmtstate = 1;
                ++le;
              }
              if (cmtstate > 0 &&  le[0] == '*' && le[1] == '/')
              { cmtstate = 0;
                ++le;
              }
            }

            ln = le+1;
            lastword[0] = 0;
          /*printf("CLW:%s", ln);*/
          }
        }}
        if (cmtstate < 0)
          cmtstate = 0;
        if (ftype != 'F' || buf[5] != '+')
          inhibit = 0;
      }    
      fclose(fip);
    }
  }}

  return 0;
}}

