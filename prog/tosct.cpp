#include <stdio.h>
#include <ctype.h>
#include <string>
#include <map>

#include "build.h"


using namespace std;

#define SHO_STATE 1



#define MAXLN 1024
#define MAXINDENT 80

static char ipline[MAXLN+4+MAXINDENT];
static char ipline_[MAXLN+4+MAXINDENT];

static void explain()
  
{ fputs( 
	"tosct {-r} {-a includesfile} {-f fieldmapfile} {-o file} {-ob} filename*\n"
	"   -a    infer arrays from include file text\n"
	"   -f    read field names from file (as TYPE name ... END TYPE name)\n"
        "   -r    right align comments\n"
        "   -lc   fields in lower case\n"
	"   -o    output file (not buffered)\n"
	"   -ob   buffer output to input filename\n"
	"   -v    verbose",
         stderr);
  exit(0);
}


Vint line_width = 80;

Vint indent_sz = 2;
Vint cmt_offs = 40;
Vint tab_width_in = 8;
Vint tab_width_out = 8;
Vint tab_width_in_is_power_of_2 = true;
Bool expand_tabs = false;
Bool v_opt = false;
Bool use_lc = false;

Vint dyn_p_offs;		/* distance from first non-space to lparen */
Vint dyn_p_ind; 		/* indent at start of line for above */
Vint rel_offs;			/* relative offset between two lines */

Bool sho_state = false;

Set16 in_cmt = 0;
#define M_AST 1
#define M_SL  2
#define M_STR 4
#define M_CHR 8


Vint parens = 0;	/* parenthesis nesting level */
Vint equs = 0;	    /* # of = we are on the right hand side of */

Vint nest = 0;
Vint swindent = 0;	/* # switches in progress */

#define MAXSTK 1000

Vint noinc[MAXSTK];	/* number of false increments at this level */
					/*[preincrement of nest]*/
Vint swtch[MAXSTK];	/* # switches in to bracket ; [preincrement of nest]*/

Vint postp[MAXSTK];	/* # post paren increments */


Bool in_noinc_ctx = false;		/* in no increment nest context */
Vint in_postp_ctx = 0;			/* # post right paren contexts */
Bool ind_nxt_stmt = false;		/* indent the next stmt */



Vint hashoffs[16];			/* queue of offsets in previous lines */
Vint cmtoffs[16];

Vint offslen = 0;
Vint offsix = 0;



Vint skipped_spaces;				/* secondary result */

Char * skipspaces(Char * str)
  
{ skipped_spaces = 0;
  
  for ( ; *str != 0 and isspace(*str); ++str)
  { skipped_spaces += 1;
    if	    (*str == '\t')
    { skipped_spaces += (tab_width_in - 1);
      if (tab_width_in_is_power_of_2)
	skipped_spaces &= ~(tab_width_in-1);
      else
      { skipped_spaces /= tab_width_in;
	skipped_spaces *= tab_width_in;
      }
    }
  }
  return str;
}

class Istring : public string
{ 

 public:
       Istring();
       Istring(const char*);
       Istring(const string &);
  Bool equal_ic(const char *) const;
  Bool equal_ic(const Istring &) const;
  void to_lower();
  void to_upper();
//Istring operator = (const string & v) ;
};

Istring::Istring():string("")

{
}


Istring::Istring(const char*s):string(s)

{
}


Istring::Istring(const string & s):string(s)

{
}


Bool Istring::equal_ic(const char * s) const

{ if (length() != strlen(s))
    return false;
  const char * t = c_str();

  while (*t != 0 && toupper(*t) == toupper(*s))
  { ++t;
    ++s;
  }

  return *s == *t;
}


Bool Istring::equal_ic(const Istring & r) const

{ if (length() != r.length())
    return false;
  char * t = const_cast<char*>(c_str());
  const char * s = r.c_str();

  while (*t != 0 && toupper(*t) == toupper(*s))
  { ++t;
    ++s;
  }

  return *s == *t;
}

void Istring::to_lower()

{ 
  char * t = const_cast<char*>(c_str());
  for (; *t != 0; ++t)
    if (*t >= 'A' && *t <= 'Z')
      *t += 'a' - 'A';
}

void Istring::to_upper()

{ 
  char * t = const_cast<char*>(c_str());
  for (; *t != 0; ++t)
    if (*t >= 'a' && *t <= 'z')
      *t += 'A' - 'a';
}



Char * get_id(Istring & id, Char * & ln_)

{ char * ln = ln_;

  while (*ln != 0 && isspace(*ln))
    ++ln;

  if (*ln == 0)
  { id = Istring("");
    return ln;
  }
  
{ Char * ee = ln+1;
  if      (isalpha(*ln) || isdigit(*ln))
  { while (*ee != 0 && (isalpha(*ee) || isdigit(*ee) || *ee == '_'))
      ++ee;
    //printf("Tail:%s\n",ee);
  }
  else if (*ln == '.')
  {
    if      (toupper(ln[1]) == 'E' && toupper(ln[2]) == 'Q' && ln[3] == '.')
    { ln[0] = ' '; ln[1] = '='; ln[2] = '='; ln[3] = ' '; }
    else if (toupper(ln[1]) == 'N' && toupper(ln[2]) == 'E' && ln[3] == '.')
    { ln[0] = ' '; ln[1] = '/'; ln[2] = '='; ln[3] = ' '; }
    else if (toupper(ln[1]) == 'G' && toupper(ln[2]) == 'E' && ln[3] == '.')
    { ln[0] = ' '; ln[1] = '>'; ln[2] = '='; ln[3] = ' '; }
    else if (toupper(ln[1]) == 'L' && toupper(ln[2]) == 'E' && ln[3] == '.')
    { ln[0] = ' '; ln[1] = '<'; ln[2] = '='; ln[3] = ' '; }
    else if (toupper(ln[1]) == 'G' && toupper(ln[2]) == 'T' && ln[3] == '.')
    { ln[0] = ' '; ln[1] = '>'; ln[2] = ' '; ln[3] = ' '; }
    else if (toupper(ln[1]) == 'L' && toupper(ln[2]) == 'T' && ln[3] == '.')
    { ln[0] = ' '; ln[1] = '<'; ln[2] = ' '; ln[3] = ' '; }

    if (*ln == ' ')
    { if (ln > ipline+4 && (ln-1)[0] == ' ')
      { strcpy(ln-1,ln);
        if (ln[3] == ' ')
          strcpy(ln+2,ln+3);
      }
      else
        ++ln; 
      ee = ln + 2;
    }
  }
{ Char sch = *ee;
  *ee = 0;
  id = Istring(ln);
  id.to_lower();
  *ee = sch;
  ln_ = ee;
  return ln;
}}}



void putspaces(int n)

{ Vint ct = expand_tabs ? 0 : n / tab_width_out;
  n -= ct * tab_width_out;
  while (--ct >= 0)
    fputc('\t', stdout);
  
  while (--n >= 0)
    fputc(' ', stdout);
}


map<Istring,int> local_dict;


void add_local(const Istring & id_str)

{ local_dict[id_str] = 1;
}


void del_local(const Istring & id_str)

{ map<Istring, int>::iterator f = local_dict.find(id_str);
  if (f != local_dict.end())
    local_dict.erase(f);
}



void mk_lower_id(char * t)

{ while (*t != 0 && (isalpha(*t) || isdigit(*t) || *t == '_'))
  { if (*t >= 'A' && *t <= 'Z') 
      *t = tolower(*t);
    ++t;
  }
}


void mk_upper_id(char * t)

{ while (*t != 0 && (isalpha(*t) || isdigit(*t) || *t == '_'))
  { if (*t >= 'a' && *t <= 'z') 
      *t = toupper(*t);
    ++t;
  }
}


Bool is_local(const Istring & s)

{ map<Istring, int>::iterator f = local_dict.find(s);

  return f != local_dict.end();
}


map<Istring,int> arr_dict;


void arr_dict_init()

{
#if 0
  arr_dict["works_operation_i39"] = 1;
  arr_dict["works_order_i40"] = 1;
#endif
}


int find_array(const Istring & s)

{ map<Istring, int>::iterator f = arr_dict.find(s);

  return f == arr_dict.end() ? 0 : f->second;
}


void add_array(const Istring & s)

{ arr_dict[s] = 1;
}


void load_arrays(char * fn)

{ FILE * chan = fopen(fn, "r");
    
  if (chan == null)
  { fputs("Cannot open file ", stderr);
    fputs(fn, stderr);
    fputs("\n", stderr);
    explain();
    return;
  }

  while (true)
  { int slen;
    char * ln;
    Char * ln_ = fgets(&ipline[4], MAXLN, chan);
    if (ln_ == null)
      break;
    slen = strlen(ln_);
    if (slen <= 0) continue;
    ln_[slen-1] = 0;

    ln = ln_;

    Istring id_str;

    char * w1 = get_id(id_str,ln);
//                                   INTEGER*4 ,ALLOCATABLE :: ROUND_I3(:,:)
    if (id_str == "integer" || id_str == "character" || id_str == "type")
    { //printf("Got intchar\n");
      if (id_str == "type")
      { 
        while (*++ln != 0 && *ln != ')')
          ;

        if (*ln != 0) ++ln;
      }
      else
      { if (ln[0] == '*' && in_range(ln[1],'1','9'))
          while (*++ln >= '0' && *ln <= '9')
            ;
      }

      ln = skipspaces(ln);
      if (*ln == ',')
      { ++ln;
        w1 = get_id(id_str,ln);
        if (id_str == "allocatable")
        { 
          ln = skipspaces(ln);
          if (ln[0] == ':' && ln[1] == ':')
          { ln += 2;
            w1 = get_id(id_str,ln);
            if (isalpha(*w1))
            { 
              const char * w = id_str.c_str();
              int wl = id_str.length();
              while (--wl > 0 && in_range(w[wl],'0','9'))
                ;
              if (wl > 3 && toupper(w[wl]) == 'I' && w[wl-1] == '_')
              { ln = skipspaces(ln);
                if (!strncmp("(:,:)",ln,5) || !strncmp("(:)",ln,3))
                { arr_dict[id_str] = 1;
                //printf("Add Arr %s\n", id_str.c_str());
                }
              }
            }
          }
        }
      }
    }
  }

  fclose(chan);
}

map<Istring,int> farr_dict;
map<Istring,Istring> fld_dict;

void load_fields(char * fn)

{ FILE * chan = fopen(fn, "r");
    
  if (chan == null)
  { fputs("Cannot open file ", stderr);
    fputs(fn, stderr);
    fputs("\n", stderr);
    explain();
    return;
  }

  int fld_no = -1;
  Istring cmd,arr_id,fld_id,num_id;

  while (true)
  { int slen;
    char * ln;
    Char * ln_ = fgets(&ipline[4], MAXLN, chan);
    if (ln_ == null)
      break;
    slen = strlen(ln_);
    if (slen <= 0) continue;
    ln_[slen-1] = 0;

    ln = ln_;
#if 0
    char * w1 = get_id(arr_id,ln);
    ln = skipspaces(ln);
    if (*ln != '.') continue;

    ++ln;
    char * w2 = get_id(fld_id,ln);

    ln = skipspaces(ln);
    if (*ln != '=') continue;

    ++ln;
    char * w3 = get_id(num_id,ln);

    char buf[132];

    if (arr_id.length() + fld_id.length() >= 130) continue;
    
    strcpy(buf,arr_id.c_str());

    farr_dict[buf] = 1;

    strcat(buf,".");
    strcat(buf,num_id.c_str());

  //printf("Flddict+=%s\n",buf);
    fld_dict[buf] = fld_id;
#else
    char * w1 = get_id(cmd,ln);

    if      (fld_no < 0) 
    { if (cmd == "type")
      { w1 = get_id(arr_id,ln);
        if (arr_id.substr(0,2) == "t_")
          arr_id.replace(0,2,"",0);

      //printf("Garr:%s\n",arr_id.c_str());
        fld_no = 0;
      }
    }
    else if (cmd == "end")
    { 
      char * w2 = get_id(fld_id,ln);
      if (fld_id == "type")
        fld_no = -1;
    }
    else
    { if (ln[0] == '*')
      while (in_range(*++ln,'0','9'))
        ;
      char * w2 = get_id(fld_id,ln);
      fld_id.to_upper();
      if (use_lc)
      { char sc = fld_id[0];
        fld_id.to_lower();
        fld_id[0] = sc;
      }

      if (ln[0] == '(')
      {  fld_id += '(';
         while (*++ln != 0 && *ln != ')')
           fld_id += *ln;
         fld_id += ')';
      }

      char buf[132];

      if (arr_id.length() + fld_id.length() >= 130) continue;
    
      strcpy(buf,arr_id.c_str());

      farr_dict[arr_id] = 1;

      sprintf(buf,"%s.%d",arr_id.c_str(),++fld_no);

    //printf("Flddict+=%s = %s\n",buf,fld_id.c_str());
      fld_dict[buf] = fld_id;
    }
#endif
  }

  fclose(chan);
}


int find_fld_arr(Istring & id_str)

{ map<Istring, int>::iterator f = farr_dict.find(id_str);

  return f == farr_dict.end() ? 0 : f->second;
}

Istring find_fld(Istring & id_str, const char * fldno)

{ Istring id = id_str;
  id += ".";
  id += fldno;

//printf("Flddict ?%s\n",id.c_str());

  map<Istring, Istring>::iterator f = fld_dict.find(id);

  if (f == fld_dict.end())
  { Char buf[132];
    sprintf(buf, "ILL%s",id.c_str());
    return buf;
  }

  return f->second;
}




void main(short argc, char ** argv)

{ Bool didfile = false;
  Bool in_declare = false;
  Bool in_vardec = false;
  Bool buffered = false;
  Bool got_flds = false;
  int  prev_line_cmt;
  int  right_a = 0;
  int  last_shift = -1;
  int  in_para = 0;
  FILE * op = stdout;
  
  Int argsleft = argc - 1;
  Char ** argv_ = &argv[0];
  
  for ( ; ; --argsleft)
  { ++argv_;
    
    if	    (argsleft <= 0)
    { if (didfile)
	break;
    }
    else if (argv_[0][0] == '-' && argv_[0][1] != 0)
    { if      (in_range(argv_[0][1], '0', '9'))
      { indent_sz = atoi(&argv_[0][1]);
	continue;
      }
      else if (argv_[0][1] == 'r')
      { right_a = 1;
        continue;
      }
      else if (argv_[0][1] == 'R')
      { right_a = 2;
        continue;
      }
      else if (argv_[0][1] == 'l' && argv_[0][2] == 'c')
      { use_lc = true;
        continue;
      }
      else if (argv_[0][1] == 'v')
      { v_opt = true;
        continue;
      }
      else if (argv_[0][1] == 'a' && argsleft > 0)
      { --argsleft;
        ++argv_;
        load_arrays(argv_[0]);
        got_flds = true;
	continue;
      }
      else if (argv_[0][1] == 'f' && argsleft > 0)
      { --argsleft;
        ++argv_;
        load_fields(argv_[0]);
        got_flds = true;
	continue;
      }
      else if (argv_[0][1] == 'o')
      { char * ofn = "a1z2q3g4.tmp";
        if (argsleft <= 0 || argv_[1][0] != '-' || argv_[0][2] == 'b')
          buffered = true;
        else
        { --argsleft;
          ++argv_;
          ofn = argv_[0];
          buffered = false;
        } 
        op = fopen(ofn,"w");
        if (op == NULL)
        { fputs("Cannot open file ", stderr);
          fputs(argv_[0], stderr);
          fputs("\n", stderr);
          explain();
        }
	continue;
      } 
      else if (argv_[0][1] == 'c')
      { cmt_offs = -1;
        if      (in_range(argv_[0][2],'0','9'))
          cmt_offs = atoi(&argv_[0][2]);
        else if (argsleft > 0 && in_range(argv_[1][0],'0','9'))
        { --argsleft;
          ++argv_;
          cmt_offs = atoi(argv_[0]);
        }
        else
          explain();
	continue;
      }
      else
      { explain();
      }
    }
    
    arr_dict_init();

    didfile = true;
  { Char * fn = argv_[0];
    if (argsleft <= 0)
      fn = "-";
    
  { FILE * chan = strcmp(fn, "-") == 0 ? stdin : fopen(fn, "r");
    Istring id_str;
    prev_line_cmt = 0;
    
    if (chan == null)
    { fputs("Cannot open file ", stderr);
      fputs(fn, stderr);
      fputs("\n", stderr);
      continue;
    }
    
    if (use_lc && !got_flds)
    while (true)
    { int slen;
      char * ln;
      Char * ln_ = fgets(&ipline[4], MAXLN, chan);
      if (ln_ == null)
        break;
      slen = strlen(ln_);
      if (slen <= 0) continue;

      while (--slen > 0 && ln_[slen-1] == ' ')
        ;

      ln_[slen] = 0;

      ln = ln_;

      if (toupper(ln[0]) != 'C')
      { while (*++ln != 0)
        { if (*ln == '.' && ln[1] == 'F' && isdigit(ln[2]))
          { for (ln += 2; *++ln != 0 && isdigit(*ln); )
             ;
            if (*ln == '_')
            { while (*++ln != 0 && (*ln == '_' || isalnum(*ln)))
                *ln = tolower(*ln);
            }
          }
        }
      }
      else
      { char * p, *w1;
        int nshift = 0;
        ++ln;

        if ((right_a > 1 || *ln == '*' && right_a > 0) && strlen(ln) <= 77)
        { if (last_shift > 0)
            nshift = last_shift;
          else
          { for (p = ln; *++p != 0 && isspace(*p); )
              ;
            if (p - ln > 3)
            { nshift = 80 - strlen(ln_);
              if (nshift < 0)
                nshift = 0;
            }
          }
          if (nshift > 0)
          { strcpy(ipline_+4,ipline+4);
            memset(ipline+6,' ',nshift);
            strcpy(ipline+6+nshift,ipline_+6);
          }
        }

        if (*ln == '*')
        { for (w1 = ipline+6+nshift; *++w1 != 0; )
            if (in_range(*w1, 'A','Z'))
              *w1 += 'a' - 'A';
          if (!prev_line_cmt)
            for (w1 = ipline+6+nshift; *++w1 != 0; )
              if (in_range(*w1, 'a','z'))
              { *w1 += 'A' - 'a';
                break;
              }
        }
      }
      prev_line_cmt = toupper(ipline[4]) == 'C' && 
                      toupper(ipline[5]) == '*';
      fprintf(op,"%s\n",ipline+4);
    }
    else
    while (true)
    { Bool inq = false;
      Bool sq = false;
      int slen;
      char * ln;
      char * w1;
      //char * w2;
      //char * w3;
      Char * ln_ = fgets(&ipline[4], MAXLN, chan);
      if (ln_ == null)
        break;
      slen = strlen(ln_);
      if (slen <= 0) continue;

      while (--slen > 0 && ln_[slen-1] == ' ')
        ;

      ln_[slen] = 0;

      ln = ln_;

      if (toupper(ln[0]) == 'C')
      { char * p;
        int nshift = 0;
        ++ln;

        if ((right_a > 1 || *ln == '*' && right_a > 0) && strlen(ln) <= 77)
        { if (last_shift > 0)
            nshift = last_shift;
          else
          { for (p = ln; *++p != 0 && isspace(*p); )
              ;
            if (p - ln > 3)
            { nshift = 80 - strlen(ln_);
              if (nshift < 0)
                nshift = 0;
            }
          }
          if (nshift > 0)
          { strcpy(ipline_+4,ipline+4);
            memset(ipline+6,' ',nshift);
            strcpy(ipline+6+nshift,ipline_+6);
          }
        }

        if (use_lc && *ln == '*')
        { for (w1 = ipline+6+nshift; *++w1 != 0; )
            if (in_range(*w1, 'A','Z'))
              *w1 += 'a' - 'A';
          if (!prev_line_cmt)
            for (w1 = ipline+6+nshift; *++w1 != 0; )
              if (in_range(*w1, 'a','z'))
              { *w1 += 'A' - 'a';
                break;
              }
        }

        last_shift = nshift;

        w1 = get_id(id_str,ln);

        if (id_str == "functions")
        { //printf("Left implicit\n");
          in_vardec = false;
          ++ln;
        }
      }
      else if (ln[0] == '#')
        ;
      else
      { last_shift = -1;
        if (slen < 5 || ln[0] == 9 || ln[5] == ' ')
        { in_declare = false;
        //printf("Outdec:%s\n", ln);
        }
        else
          sq = true;
  
        w1 = get_id(id_str,ln);
  
        if (*w1 == '!' || *w1 == 0)
          ;
        else if      (id_str == "implicit")
        {//printf("Got implicit\n");
          local_dict.clear();
          in_vardec = true;
          in_para = 0;
          w1 = get_id(id_str,ln);
        }
        else if (id_str == "use")
          in_para = 0;
        else if (id_str=="common" || id_str=="data" || id_str=="automatic")
        { 
          w1 = get_id(id_str,ln);
        }
  
        else if ((id_str == "integer" || id_str == "character" ||
                  id_str == "logical" || id_str == "real")
               && ln[0] == '*'
               && (in_range(ln[1],'1','9')|| ln[1] == '('))
        { 
          //printf("Indeclare:\n");
          in_declare = true;
          ++ln;
          w1 = get_id(id_str,ln);
        }
        else if (id_str == "record" || id_str == "type" )
        { 
          in_declare = true;
          w1 = get_id(id_str,ln); // / or (
          w1 = get_id(id_str,ln); // id
          w1 = get_id(id_str,ln); // / or )
        }
        else if (id_str == "parameter" )
        { ln = skipspaces(ln);
          if (*ln == '(')
          { ++ln;
            char * idd = get_id(id_str,ln);
            del_local(id_str);
            in_declare = false;
          }
        }
        else if (id_str == "call" || id_str == "if" || id_str == "do" ||
                 id_str == "select" )
          in_vardec = false;

        while (*w1 != 0 && *w1 != '!')
        { 
        //printf("Id:%s\n", id_str.c_str());
  
          if      (*w1 == '\'')
            inq = !inq;
          else if (*w1 == ')')
            in_para = 0;
          else if (isalpha(*w1) && !inq)
            if      (id_str == "function" || id_str == "subroutine")
            { in_para = 1;
              local_dict.clear();
              w1 = get_id(id_str,ln);
              if (*w1 == 0 || *w1 == '!')
                break;
            }
            else if (id_str == "parameter" )
            { ln = skipspaces(ln);
              if (*ln == '(')
              { ++ln;
                char * idd = get_id(id_str,ln);
                del_local(id_str);
                in_declare = false;
                for (w1 = ln_; *++w1 != 0 && !isdigit(*w1); )
                  ;
                if (*w1 != 0) 
                { w1 = skipspaces(w1+1);
                  if (strncmp(w1,id_str.c_str(),id_str.length()) == 0)
                    mk_upper_id(w1);
                }
              }
            }
            else if (in_declare && in_vardec)
            { //printf("Got declare: %s\n",id_str.c_str());
              if (id_str != "allocatable" 
               && id_str != "save"
               && id_str != "automatic")
              { add_local(id_str);
                mk_lower_id(w1);
              }
            }
            else if (is_local(id_str) || in_para != 0)
            { 
              mk_lower_id(w1);
            }
          //else
            //printf("Leave: %d %d %s\n",in_declare,in_vardec,id_str.c_str());

          w1 = get_id(id_str,ln);
        }
 
        for (int iter = 2; --iter >= 0; )
        { ln = ln_;
          while (1)
          {
            w1 = get_id(id_str,ln);
            if (*w1 == 0) break;
        
          { Bool got = false;
            int arr_id = find_array(id_str);
            if (arr_id != 0)
            {
              char * ln1 = skipspaces(ln);
              if (*ln1 == '(' && (ln1[1] == 'I' || ln1[1] == 'i')
                                && isdigit(ln1[2]))
              { char * ln2;
                char * ln3 = ln1+1;
                char * probe = ln1+2;
                int indent = 0;
                while (in_range(*probe, '0','9'))
                  ++probe;
                if (*probe != '_')
                  indent = -1;
                for (ln2 = ln1 + 1; indent >= 0 && *++ln2 != 0; )
                { if (*ln2 == '(') ++indent;
                  if (*ln2 == ')') --indent;
                  if (*ln2 == ',' && indent == 0)
                    ln3 = ln2;
                }
    
                if (*ln3 == ',' && *ln2 == ')')
                { char buf[290];
                  memcpy(buf,ln1+1,ln3-ln1-1);  // to , exclusive
                  buf[ln3-ln1-1] = 0;
                  //printf("Buf:%s\n",buf);
                  memcpy(ln1+1,ln3+1,ln2-ln3);  // to ) inclusive
                  ln1[1+ln2-ln3] = '.';
                  //printf("Ip:%s\n",ln_);
                  memcpy(ln1+1+(ln2-ln3)+1,buf,strlen(buf));
                  ln = ln2;
                  got = true;
                }
              }
            }
            if (!got)
            { int arr_id = find_fld_arr(id_str);
              if (arr_id != 0)
              {
                char * ln1 = skipspaces(ln);
                if (*ln1 == '(' && isdigit(ln1[1]))
                { char * eix = NULL;
                  char * ln2;
                  char * comma = NULL;
                  int indent = 0;
                  for (ln2 = ln1+1; indent >= 0 && *++ln2 != 0; )
                  { if (*ln2 == '(') ++indent;
                    if (*ln2 == ')') --indent;
                    if (*ln2 == ',' && indent == 0 && comma == NULL)
                      comma = ln2;
                    if (*ln2 == '+' && comma == eix && indent == 0)
                      eix = ln2;
                  }
      
                  if (comma != NULL && *ln2 == ')' && ln2[1] != '.')
                  { char buf[290];
                    char tail[290];
                    if (strlen(ln2) >= sizeof(buf))
                      strcpy(tail,"OVERFLOW");
                    else
                      memcpy(tail,ln2+1,strlen(ln2)+1);
  
                    int fldno = atoi(ln1+1);
                    string eixs;
                    if (eix != NULL)
                    { ++fldno;
                      memcpy(buf,eix+1,comma - eix - 1);
                             buf[      comma - eix - 1] = 0;
                      eixs = buf;
                    }
                    sprintf(buf,"%d",fldno);
                    //printf("Buf:%s\n",buf);
                    memcpy(ln1+1,comma+1,ln2-comma);  // to ) inclusive

                    char * e = ln1+1+(ln2-comma);
                      
                    *e = '.';
                    //printf("Ip:%s\n",ln_);
  
                    Istring fld = find_fld(id_str,buf).c_str();
                    if (eix != NULL)
                    { int ix;
                      for (ix = fld.length(); --ix > 0; )
                        if (fld[ix] == '(') break;
                      if (ix > 0)
                      { 
                        fld = fld.substr(0,ix);
                      //printf("Eif:%s\n",fld.c_str());
                      }
                    }
                  
                    memcpy(e+1,fld.c_str(),fld.length());
                    if (eix != NULL)
                    { e[1+fld.length()] = '(';
                      memcpy(e+2+fld.length(),eixs.c_str(),eixs.length());
                      e += eixs.length();
                      e[2+fld.length()] = ')';
                      e += 2;
                    }
                    memcpy(e+1+fld.length(),tail,strlen(tail));
                    e[1+fld.length()+     strlen(tail)]=0;
                  //printf("Buf:%s\n",tail);
                    if (tail[0] == 0)
                      ln = "";
                    else
                      ln = e+1+fld.length();
                  //printf("Now: %s\n",ipline+4);
                  }
                }
              }
            }
          }} // over buf
        } // iter
      }
      prev_line_cmt = toupper(ipline[4]) == 'C' && 
                      toupper(ipline[5]) == '*';
      fprintf(op,"%s\n",ipline+4);
    }
    
    fclose(chan);
    fclose(op);

    if (buffered)
    {
#if S_MSDOS
      sprintf(ipline,"move a1z2q3g4.tmp %s",fn);
#else
      sprintf(ipline,"mv a1z2q3g4.tmp %s",fn);
#endif
      system(ipline);
      if (v_opt)
        printf("Replaced %s\n", fn);
    }
   
    if (argsleft <= 0)
      break;
  }}}
}
