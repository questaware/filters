/*tab 4*/
/*****************************************************************************
 */

#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../h/base.h"
#undef public
#undef private

using namespace std;

#define MIN_PG_BUF_SZ 20000

#include "extrhtml.h"



#define SALONE 1


											/* Warning: that deleting an object 
												also deletes all the next objects */
Active_obj::~Active_obj()
{ Active_obj * sub = children;

  printf("DM %x\n", this);
  if (sub != NULL)   
  { printf("DH %x\n", sub);
    delete sub;
    children = NULL;
  }

  Active_obj * lst = next; 
  while (lst != NULL)
  { Active_obj * nxt = lst->next;
    lst->next = NULL;
    printf("DR %x\n", lst);
    delete lst;
    lst = nxt;
  }
  next = NULL;
  printf("DHone\n");
};



void Active_obj::print_obj(int indent)
{ for (int i = indent; --i >= 0; )
    printf(" ");
  printf("Kind %s[%s](%d)	name %s	value = %s	href %s\n", kind == e_form   ? "F" :
					  										kind == e_a      ? "A" :
															kind == e_input  ? "I" :
															kind == e_iframe ? "U" : "O",
															i_type == it_file  ? "fi" :
					  										i_type == it_text || 
					  										i_type == it_radio ? "te" : 
															i_type == it_submit?"su" : "nk",
		maxlength, 
		name==NULL ? "" : name, value==NULL ? "" : value, href==NULL ? "" : href);
  Active_obj * sub = children;
  for (; sub != NULL; sub = sub->next)
   sub->print_obj(indent+1); 
}





void Ao_list::append(Active_obj * obj) 
{ if (tail == NULL) 
  { head = obj; 
    tail = obj;
    obj->next = NULL;
  }
  else
  { tail->next = obj;
    tail = obj;
  }
}



char * cb_buf;
int    cb_max_size;
int    cb_size;

void * new_cb(int sz)

{ cb_max_size = sz;
  cb_size = 0;
  cb_buf = (char *)malloc(sz+20);
  if (cb_buf == NULL)
  { printf("Error: out of mem\n");
    return NULL;
  }
  cb_buf[0] = 0;
  return (void*)write_cb;
}


void fin_cb()

{ if (cb_buf != NULL)
  { free(cb_buf);
    cb_buf = NULL;
  }
}



/* curl write callback */
int write_cb(char *in, int size, int nmemb, Char * type)
{
  int sz = size * nmemb;
  if (cb_size + sz >= cb_max_size)
  { int ocbs = cb_size;
    char * ocb = cb_buf;
    (void)new_cb(cb_max_size + MIN_PG_BUF_SZ);
    if (cb_buf == NULL)
      return 0;
    memcpy(cb_buf, ocb, ocbs);
    cb_size = ocbs;
  }

  memcpy(&cb_buf[cb_size], in, sz);
  cb_size += sz;
  cb_buf[cb_size] = 0;
  return sz;
}



char * string_skip(const char * s_)

{ const char * s = s_;
  char delim = *s++;
  for (; *s != 0 && *s != delim; ++s)
   if (*s == '\\' && *s == delim)
     ++s;
  return const_cast<char*>(*s == 0 ? s : s + 1);
}


char * string_get(char * & res, const char * s_)

{ const char * s = s_;
  char delim = *s++;
  for (; *s != 0 && *s != delim; ++s)
   if (*s == '\\' && *s == delim)
     ++s;

  res = (char*)malloc(s - s_);
  if (res != NULL)
  { memcpy(res, s_+1, s - s_ - 1);
    res[s - s_ - 1] = 0;
  }

  return const_cast<char*>(*s == 0 ? s : s + 1);
}


char * get_name_end(const char * s)

{// for (; *s != 0 && (isalpha(*s) || isdigit(*s)); ++s)
  for (; *s != 0 && !isspace(*s) && *s != '>' && *s != '<'; ++s)
   ;
  return const_cast<char*>(s);
}


char * get_token_dup(char * & res, const char * s)

{ while (*s != 0 && isspace(*s))
    ++s;
  char * ss = get_name_end(s);
  res = (char*)malloc(ss + 1 - s);
  if (res != NULL)
  { memcpy(res, s, ss - s);
    res[ss - s] = 0;
  }
  return ss;
}



char * get_ts_dup(char * & res, const char * s)

{ return const_cast<char*>(s[0] == '"' || s[0] == '\'' ? string_get(res, s)
							 	       				   : get_token_dup(res, s));
}


char * skip_ts(const char * s)

{ char * trash = NULL;
  char * ss = get_ts_dup(trash, s);
  if (trash != NULL)
    free(trash);
  return ss;
}



char * skip_to_lt(const char * s_)

{ const char * s = s_;
  for (; *s != 0 && *s != '<'; ++s)
   if (*s == '\\' && *s == '<')
     ++s;
  return const_cast<char*>(s);
}


char * get_to_lt_dup(char * & res, const char * s_)

{ const char * s = s_;
  for (; *s != 0 && *s != '<'; ++s)
   if (*s == '\\' && *s == '<')
     ++s;

  res = (char*)malloc(s - s_ + 1);
  if (res != NULL)
  { memcpy(res, s_, s - s_);
    res[s - s_] = 0;
  }

  return const_cast<char*>(s);
}



/*
  STRMATCH:   return a pointer into t_
	      the result is the first character in t_ not matching s
 */
char * strmatch(const char *t_, const char *s_)

{ fast const char * t = t_;
  fast const char * s = s_;

  for (; *s != 0 && *t != 0 && (*t | 0x20) == (*s | 0x20); ++s)
    ++t;

  return const_cast<char*>(t);
}



/*
  STRMATCHWORD:   return a pointer into t_
	      the result is the first character in t_ not matching s 
				IF s_[t-t_] terminates a word
	      otherwise t_
 */
char * strmatchword(const char *t_, const char *s_)

{ fast const char * t = t_;
  fast const char * s = s_;

  for (; *s != 0 && *t != 0 && (*t | 0x20) == (*s | 0x20); ++s)
    ++t;

  if (*s != 0)
    return const_cast<char*>(t_);

  char ch = *t;
  return isalpha(ch) || isdigit(ch) || ch == '_' ? const_cast<char*>(t_)
												 : const_cast<char*>(t);
}





void Requests::add_req(const Request & rq)

{ if (rq.kind == e_fixedtxt)
  { got_fixedtxt = true;
  
    unsigned char ch = (unsigned char)rq.names[0];
    set_fixed_text[ch >> 3] |= (1 << (ch & 7));
  }
  reqs.push_back(rq);	
}



int Requests::find_name(const char * * resptr, const char * name, Obj_Type kind)

{ int ix = 0;
 
  if (name == NULL)
    return -1;

  if (name[0] == '"')
    ++name;

  printf("FindName %20.20s\n", name);

  for (std::vector <Request>:: const_iterator curr = reqs.begin();
       curr != reqs.end();
       ++curr, ++ix)
  { if (kind != curr->kind && kind != 0)
	  continue;

//  if (kind == e_fixedtxt)
//	  printf("STRMATCH %20.20s XX %s\n", name, curr->names);

    char * fd = strmatchword(name, curr->names);
    if (*fd == 0)
	  return -1;

    if (fd != name)
	{ int nix = curr->noo_names; 
      if (nix != 1)
	  { //printf("GOOD %d %20.20s\n", nix, name);
	    const char * fd_ = name;
	    const char * cn = curr->names;
	    for (; --nix > 0; )
	    { cn += fd - fd_ + 1;
          while (*fd != 0 && isspace(*fd))
		    ++fd;
		
	      //printf("GSM %20.20s %20.20s\n", fd, cn);
		  fd_ = fd;
		  fd = strmatch(fd, cn);
          if (fd - fd_ != strlen(cn))
//        if (*fd == 0 || isalpha(*fd) || isdigit(*fd) || *fd == '_')
	      { //printf("break on %d %20.20s\n", nix, fd);
		    break;
		  }
		}
		++nix;
	  }
	  if (nix == 1)
	  { printf("FNFD %20.20s\n", fd);
		if (resptr != NULL)
		  *resptr = fd;
	    return ix;
	  } 
	}
  }

  return -1;
}



void Requests::clear_results()

{ for (std::vector <Request>:: iterator curr = reqs.begin();
       curr != reqs.end();
       ++curr)
  { if (curr->value != NULL)
      curr->value = NULL;
     
    if (curr->href != NULL)
      curr->href = NULL;
  }
}



Cc Requests::save(char * fname, int fix, const char * value, const char * href)

{ if (fix >= reqs.size())
  { printf("Int Err. 3\n", fix);
    return -1;
  }

  char * fn = reqs[fix].reqform;
  printf("Stry %s %20.20s (%s)\n", reqs[fix].names, fname, fn);

  if (fname != NULL && fname[0] != 0 && fn[0] != 0)
  { char * match = strmatchword(fname, fn);
    if (match == fn)
    { //printf("Rejected %20.20s (%s)\n", fname, fn);
      return OK;
    }
  }

  bool check = false;

  if (value != NULL)
  { if (reqs[fix].value != NULL && reqs[fix].value != value && reqs[fix].actform[0] != 0)
	  check = true;
    reqs[fix].value = const_cast<char*>(value);
  }
  if (href != NULL)
  { if (reqs[fix].href != NULL && reqs[fix].href != href && reqs[fix].actform[0] != 0)
	  check = true;
    reqs[fix].href = const_cast<char*>(href);
  }

  if (fname != NULL && fname[0] != 0)
    if (check)
	{ if (strmatchword(fname, reqs[fix].actform) == fname)
	  { reqs[fix].error = 1;
	    printf("Multiple Name %s Actform %s Thisform %20.20s\n", 
				reqs[fix].names, reqs[fix].actform, fname);
      }
    }
    else
    { char * fnm = NULL;
      get_ts_dup(fnm, fname);
      if (fnm != NULL)
      { int sl = strlen(fnm);
        if (sl >= sizeof(reqs[fix].actform))
		  fnm[sizeof(reqs[fix].actform) -1] = 0;
		strcpy(reqs[fix].actform, fnm);
		free(fnm);
      }
    }

  return OK;
}


void Requests::print()

{ for (std::vector <Request>:: const_iterator curr = reqs.begin();
       curr != reqs.end();
       ++curr)
  { char * value = curr->value;  if (value != NULL) get_ts_dup(value, value);
    char * href = curr->href;    if (href  != NULL) get_ts_dup(href, href);
	if (href == NULL)
      printf(curr->error ? "Ke %s N %s V %s\n" :
	 					   "K %s N %s V %s\n",
									curr->kind == e_input    ? "I" :
									curr->kind == e_iframe   ? "U" :
		  							curr->kind == e_select   ? "S" :
									curr->kind == e_a        ? "A" : 
									curr->kind == e_fixedtxt ? "F" : "?",
									curr->names,
									value);
	else
      printf(curr->error ? "Ke %s N %s V %s H %s\n" :
	 					   "K %s N %s V %s H %s\n",
									curr->kind == e_input    ? "I" :
									curr->kind == e_iframe   ? "U" :
		  							curr->kind == e_select   ? "S" :
									curr->kind == e_a        ? "A" : 
									curr->kind == e_fixedtxt ? "F" : "?",
									curr->names,
									value,
									href);
	if (value != NULL) free(value);
	if (href != NULL) free(href);
  } 
}



int get_type(const vector <Active_obj *> & obj_stk, Obj_Type okind)

{ int ix = obj_stk.size();
  for (; --ix >= 0; )
  { if (obj_stk[ix]->kind == okind)
	  return ix;
  }

  return -1;
}



class Parse_Stk
{
 public:
  vector <Ao_list> list_stack;
  vector <Active_obj *> obj_stack;

  bool pop_off_type(Ao_list & ol, Active_obj * & cur_obj, Obj_Type okind);

  void fini(Ao_list & ol) { Active_obj * t2; pop_off_type(ol, t2, e_none); }
};




bool Parse_Stk::pop_off_type(Ao_list & ol, Active_obj * & cur_obj, Obj_Type okind)

{ if (okind != e_none && get_type(obj_stack, okind) < 0)
    return false;

  int sz = list_stack.size();
  int ix;
  for (ix = sz; --ix >= 0; )
  { Active_obj * obj = obj_stack[ix];
    if (ol.head == obj)
    { printf("SHITTTTT\n");
    }
    obj->children = ol.head;
    ol = list_stack[ix];

    obj_stack.pop_back();
    list_stack.pop_back();
    if (obj->kind == okind)
	  break;
  }

  for (; --ix >= 0; )
    if (obj_stack[ix]->kind == okind)
	{ cur_obj = obj_stack[ix];
	  return true;
    }

  cur_obj = NULL;
  return true;
}



Cc get_active_objs(Ao_list & op, Requests & reqs, const char * buf)

{ Active_obj * obj = NULL;
  Active_obj * cur_form = NULL;
  Active_obj * cur_select = NULL;
  Ao_list obj_list;
//Ao_list input_list;
//Ao_list option_list;

  Parse_Stk ps;

  bool in_cmt = 0;
  Active_obj * selected_val = NULL;

  const char * s = buf;
  int line_no;
  for (line_no = 1; *s != 0; ++line_no)
  { char * t = const_cast<char*>(s);
    for (; *t != 0 && *t != '\n'; ++t)
      ;
/*  char sch = *t;
    *t = 0; printf("%d X %s\n", line_no, s); *t = sch;*/
      
    const char * ss = s;
    for (; *ss != 0 && *ss != '\n'; ++ss)
    { 
      if      (in_cmt)
      { if (ss[0] == '-' && ss[1] == '-' && ss[2] == '>')
        { in_cmt = false; continue;
        }
      }
      else if (*ss == '>')
      { if (obj != NULL && obj->kind == e_a)
        { int fix = reqs.find_name(NULL, ss+1, e_a);
		  if (fix >= 0)
		    reqs.save(cur_form == NULL ? NULL : cur_form->name, fix, NULL, obj->href);

		  ss = skip_to_lt(ss+1)-1; //printf("GotA\n"); 
		}
        else if (obj != NULL && obj->name != NULL && obj->i_type != it_radio)
        { int fix = reqs.find_name(NULL, obj->name, obj->kind);
		  //printf("EIf %x %20.20s\n", fix, obj->value);
		  if (fix >= 0)
		    reqs.save(cur_form == NULL ? NULL : cur_form->name, fix, obj->value, obj->href);

		  ss = skip_to_lt(ss+1)-1; //printf("GotI %x %20.20s\n", obj, obj->name); 
		}
        obj = NULL;
      }
      else if (*ss == '<')
	  { if (toupper(ss[1]) == '!' && ss[2] == '-' && ss[3] == '-')
		{ in_cmt = true;
		  continue;
		}
        int kind = 1;
		if (ss[1] == '/')
		{ ++ss;
		  if (ss[1] == '\n' || ss[1] == 0)
		    break;
		  kind = -1;
		}
	 			  
		if      (toupper(ss[1]) == 'A' && !isalpha(ss[2]) && !isdigit(ss[2]))
		  kind *= e_a;
	    else if (strnicmp(ss+1, "input", 5) == 0 && !isalpha(ss[6]) && !isdigit(ss[6]))
          kind *= e_input;
	    else if (strnicmp(ss+1, "iframe", 6) == 0 && !isalpha(ss[7]) && !isdigit(ss[7]))
          kind *= e_iframe;
	    else if (strnicmp(ss+1, "select",6) == 0 && !isalpha(ss[7]) && !isdigit(ss[7]))
		  kind *= e_select;
        else if (strnicmp(ss+1, "form", 4) == 0 && !isalpha(ss[5]) && !isdigit(ss[5]))
		  kind *= e_form;
        else if (strnicmp(ss+1, "option", 6) == 0 && !isalpha(ss[7]) && !isdigit(ss[7]))
		  kind *= e_option;
        else
		  continue;
	
		//printf("Got here %20.20s %d\n", ss, kind);
		if (kind < 0)
		{ 
		  if (kind == -e_select)
		  { 
			//printf("Eselect %x %20.20s\n", selected_val, cur_select->name);

			if (selected_val != NULL && cur_select->name != NULL)
            { int fix = reqs.find_name(NULL, cur_select->name, e_select);
			  //printf("ESf %x %20.20s ||| %20.20s\n", fix, cur_select->name, selected_val->value);
		      //printf("CfN %20.20s\n", cur_form->name);
			  if (fix >= 0)
		        reqs.save(cur_form==NULL ? NULL : cur_form->name, fix, selected_val->value, NULL);
			}
			ps.pop_off_type(obj_list, cur_select, e_select);
        	selected_val = NULL;
		  }
		  else if (kind == -e_form)
		    ps.pop_off_type(obj_list, cur_form, e_form);
		  else
		    if (obj != NULL && obj->kind== -kind && obj->name != NULL && obj->i_type != it_radio)
            { int fix = reqs.find_name(NULL, obj->name, obj->kind);
		      if (fix >= 0)
		        reqs.save(cur_form==NULL ? NULL : cur_form->name, fix, obj->value, obj->href);
		    }	  
		}
		else
		{ obj = new Active_obj;
		  obj->kind = (Obj_Type)kind;
		  obj->s    = const_cast<char*>(ss);

		  if      (kind == e_form)
		  { ps.obj_stack. push_back(obj);
		    ps.list_stack.push_back(obj_list);
			obj_list.myinit(); obj_list.head = NULL;
		    cur_form = obj;
		  }
		  else if (kind == e_select)
		  { ps.obj_stack. push_back(obj);
		    ps.list_stack.push_back(obj_list);
			obj_list.myinit(); obj_list.head = NULL;
		    cur_select = obj;
		  }
		  else
		    obj_list.append(obj);
		}
  	  }
      else if (reqs.got_fixedtxt && 
			   reqs.set_fixed_text[((int)*ss & 0xff)>>3] & (1 << (*ss & 7)))
	  { //printf("TRYF %20.20s\n", ss); 
		int fix = reqs.find_name(&ss, ss, e_fixedtxt);
		if (fix >= 0)
	    { //printf("FD %d\n", fix); 

		  reqs.save(cur_form == NULL ? NULL : cur_form->name, fix, ss, NULL);
        }
	  }
      else if (ss[0] == '"')
        ss = string_skip(ss)-1;

      if (obj == NULL)
		continue;
					/* ignore the potential leakage! */
      if      (toupper(*ss) == 'H' && strnicmp(ss, "href=", 5) == 0)
      { //printf("GotHR %20.20s\n", ss);
        obj->href = const_cast<char*>(ss+5);
		ss = skip_ts(ss+5)-1; //printf("GotA\n"); 
      }
	  else if (toupper(*ss) == 'A' && strnicmp(ss, "action=", 7) == 0)
      { //printf("GotA %20.20s\n", ss);
        obj->href = const_cast<char*>(ss+7);
		ss = skip_ts(ss+7)-1; //printf("GotA\n"); 
      }
	  else if (toupper(*ss)=='S' && strnicmp(ss, "src=",4) == 0 && obj->kind == e_iframe)
      { //printf("GotS %20.20s\n", ss);
        obj->value = const_cast<char*>(ss+4);
		ss = skip_ts(ss+4)-1; //printf("GotA\n"); 
      }
	  else if (toupper(*ss) == 'S' && strnicmp(ss, "selected", 8) == 0)
	  { selected_val = obj; ss += 7;
		//printf("Selected %x\n", obj); 
	  }
	  else if (toupper(*ss) == 'C' && strnicmp(ss, "checked", 7) == 0)
	  { obj->i_type = it_radiochecked;
        printf("CHECKED VAL %s\n", obj->value);
	    int fix = reqs.find_name(NULL, obj->name, obj->kind);
	    if (fix >= 0)
	      reqs.save(cur_form==NULL ? NULL : cur_form->name, fix, obj->value, obj->href);
	    ss += 6;
	  }
      else if (toupper(*ss) == 'T' && strnicmp(ss, "type=", 5) == 0)
      { const char * t = ss+5; 
        if (*t == '"' || *t == '\'')
          ++t;
        obj->i_type = strnicmp(t, "file", 4)  == 0 ? it_file : 
        	    	  strnicmp(t, "text", 4)  == 0 ? it_text : 
        	    	  strnicmp(t, "radio",5)  == 0 ? it_radio: 
	        	      strnicmp(t, "submit",6) == 0 ? it_submit : it_unkn;
        printf("GotTyp %x %d\n", obj, obj==NULL ? -1 : obj->i_type);
		ss = skip_ts(ss+5)-1;
      }
      else if (toupper(*ss) == 'V' && strnicmp(ss, "value=", 6) == 0)
      { printf("GotValue %20.20s\n", ss+6);
        obj->value = const_cast<char*>(ss+6);
		ss = skip_ts(ss+6)-1;
        printf("Skipped to %20.20s\n", ss);
/*	    if (obj->i_type == it_radiochecked)
        { int fix = reqs.find_name(NULL, obj->name, obj->kind);
	      if (fix >= 0)
	        reqs.save(cur_form==NULL ? NULL : cur_form->name, fix, obj->value, obj->href);
	    } */
      }
      else if (toupper(*ss) == 'N' && strnicmp(ss, "name=", 5) == 0)
      { printf("GotName %20.20s val %20.20s\n", ss+5, obj->value);
        obj->name = const_cast<char*>(ss+5);
        
/*		if (obj->i_type != it_radio)
		{ int fix = reqs.find_name(NULL, ss+5, e_none);
		  if (fix >= 0)
		    reqs.save(cur_form == NULL ? NULL : cur_form->name, fix, obj->value, obj->href);
 		} */
		ss = skip_ts(ss+1)-1;
      }
      else if (toupper(*ss) == 'M' && strnicmp(ss, "maxlength=", 10) == 0)
      { obj->maxlength = ss[10] == '"' ? atoi(ss+11) : atoi(ss+10); /*printf("GotML\n");*/ }
      else if (toupper(*ss) == 'S' && strnicmp(ss, "size=", 5) == 0)
      { obj->size = ss[5] == '"' ? atoi(ss+6) : atoi(ss+5); /*printf("GotSZ\n");*/ }
    } /* loop */
    
    if (*ss != 0)
      ++ss;
    s = ss;
  } /* loop */

  ps.fini(obj_list);
  op = obj_list;
  return OK;
}


#if SALONE


void explain()

{ printf("extrhtml {-i name} {-s name} {-f string} {-t} filename*\n");
  printf("  -i name   -- Name of an input field\n");
  printf("  -s name   -- Name of a select\n");
  printf("  -l string -- string labelling value\n");
  printf("  -t        -- use ivc settings\n");
  exit(1);
}



int main(int argc, char **argv )

{ int argsleft = argc - 1;
  Char ** argv_ = &argv[1];

  Requests reqs;

  int a_opt = 0;
  int g_opt = 0;
  int h_opt = 0;
  int l_opt = 0;
  int n_opt = 0;
  char * formname = "";

  for (; argsleft > 0; --argsleft)
  { 
    if      (argv_[0][0] != '-' || argv_[0][1] == 0)
    { int aixx;
      char * fn = argv_[0];
/*
      for (aixx = strlen(wd); --aixx >= 0; )
        wd[aixx] = toupper(wd[aixx]);
*/    
      FILE * ip = fn[0] == '-' ? stdin : fopen(fn, "r");
	  if (ip == NULL)
	  { printf("Cannot open input file %s\n", fn);
		return -1;
	  }
      char * ln;
	  char buf[1025];
	  while (true)
	  { size_t noch = fread(buf, 1, sizeof(buf)-1, ip);
		if (noch == 0 && feof(ip))
          break;
        (void)write_cb(buf, noch, 1, NULL);
	  }  

	  Ao_list op;
      (void)get_active_objs(op, reqs, cb_buf);
	  reqs.print();
      op.deletecontents();
    }
    else if (argv_[0][1] == 'a')
	  a_opt = 1;
    else if (argv_[0][1] == 'f' && argsleft > 1)
    { --argsleft;
      ++argv_;
	  formname = argv_[0];
    }
    else if ((argv_[0][1] == 'i' || argv_[0][1] == 's' || argv_[0][1] == 'l') &&
             argsleft > 1)
    { char wh = argv_[0][1];
      --argsleft;
      ++argv_;
	  Request rq;
      char * name = argv_[0];
      if (name[0] == 0 || strlen(name) > sizeof(rq.names)-1)
		explain();
/*
  Obj_Type   kind;
  int		 noo_names;
  char       names[100];
  char	     leadin[40];
  char *     value;
  char *     href;
typedef enum {e_none, e_a, e_form, e_input, e_select, e_option, e_fixedtxt, } Obj_Type;
*/
	  rq.kind = wh == 'i' ? e_input  : 
				wh == 's' ? e_select : e_fixedtxt;
	  rq.noo_names = 0;

	  do /* once */
	  { if (rq.kind == e_fixedtxt)
	    { int noo = atoi(name);
          if (noo > 0)
		  { int nsz = 0;
			while (--noo >= 0)
			{ --argsleft;
		      if (argsleft <= 0)
		        explain();

		      ++argv_;
		      rq.noo_names += 1;
      		  name = argv_[0];
			  int sl = strlen(name);
			  if (nsz + sl > sizeof(rq.names)-1)
			    explain();

			  if (name[0] == '"')
			  { ++name;
				--sl;
			    if (name[sl-1] == '"')
				{ --sl;
				  name[sl] = 0;
				}
			  }

		      printf("PNAME is %s\n", name);
		      strcpy(&rq.names[nsz], name);
			  nsz += sl + 1;
			}
			break;
		  }
	    }

	    rq.noo_names = 1;
	    printf("NAME is %s\n", name);
	    strcpy(rq.reqform, formname);
	    strcpy(rq.names, name);
      } while (0);

	  reqs.add_req(rq);
    }
    else if (argv_[0][1] == 't')
    { char * t;
	  Request rq;
	  char * f1 = "Form1";

	  rq.kind = e_select;
	  strcpy(&rq.names[0], "Salutation");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "FirstName");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "Surname");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_select;
	  strcpy(&rq.names[0], "ClubID");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "EMail");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "HomeTelephoneNumber");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "WorkTelephoneNumber");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "MobileTelephoneNumber");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "AddressLine1");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "AddressLine2");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "AddressLine3");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "Town");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "County");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  
      rq.kind = e_input;
	  strcpy(&rq.names[0], "PostCode");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  
      rq.kind = e_iframe;
	  strcpy(&rq.names[0], "cwindow");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();

	  rq.kind = e_select;
	  strcpy(&rq.names[0], "MembershipCategory");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_fixedtxt;
	  strcpy(&rq.names[0], "Membership");
	  t = rq.names;
      t += strlen(t)+1;
	  strcpy(t, "Number:&nbsp;</FONT></TD>");
	  t += strlen(t)+1;
	  strcpy(t, "<TD><FONT class=Label>");
      rq.noo_names = 3;
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_fixedtxt;
	  strcpy(&rq.names[0], "Membership");
	  t = rq.names;
	  t += strlen(t)+1;
	  strcpy(t, "Start Date:&nbsp;</FONT></TD>");
	  t += strlen(t)+1;
	  strcpy(t, "<TD><FONT class=Label>");
      rq.noo_names = 3;
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  strcpy(&rq.names[0], "Initial Lead");
	  t = rq.names;
	  t += strlen(t)+1;
	  strcpy(t, "Source:&nbsp;</FONT></TD>");
	  t += strlen(t)+1;
	  strcpy(t, "<TD><FONT class=Label>");
      rq.noo_names = 3;
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_select;
	  strcpy(&rq.names[0], "Status");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_select;
	  strcpy(&rq.names[0], "MembershipLapseDateDay");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_select;
	  strcpy(&rq.names[0], "MembershipLapseDateMonth");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_select;
	  strcpy(&rq.names[0], "MembershipLapseDateYear");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "Age");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_select;
	  strcpy(&rq.names[0], "Day");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_select;
	  strcpy(&rq.names[0], "Month");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_select;
	  strcpy(&rq.names[0], "Year");
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "SendIntroductoryEMail");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "SendBroadcastMessages");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();

	  rq.kind = e_input;
	  strcpy(&rq.names[0], "SendRegularEMail");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();

	  rq.kind = e_input;
	  memset(rq.names, 0, sizeof(rq.names));
	  strcpy(&rq.names[0], "EMailPublish");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();

	  rq.kind = e_input;
	  strcpy(&rq.names[0], "HomeTelephonePublish");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "WorkTelephonePublish");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "MobileTelephonePublish");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
	  rq.kind = e_input;
	  strcpy(&rq.names[0], "AddressPublish");
	  strcpy(rq.reqform, f1);
	  reqs.add_req(rq); rq.reinit();
    }
    else
      explain();
    ++argv_;
  }

  printf("Done\n");
  return 0;
}

#endif


