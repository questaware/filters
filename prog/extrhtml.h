


typedef enum {e_none, e_a, e_form, e_input, e_iframe, e_select, e_option, e_fixedtxt } 
 							     Obj_Type;
typedef enum {it_none, it_file, it_text, it_radio, it_radiochecked, it_submit, it_unkn} Input_Type;


class Active_obj
{
 public:
  Obj_Type kind;
  Input_Type i_type;  
  char * s;
  int sz;
  char * name;
  char * href;
  char * value;
  int maxlength;
  int size;

  Active_obj * next;
  Active_obj * children;

  Active_obj() { s= NULL; sz = 0; i_type = it_none; maxlength = -1; size = -1;
  		 name= NULL; value = NULL; href = NULL; next= NULL; children= NULL; };

  ~Active_obj();

  void print_obj(int indent = 0);

};


class Ao_list
{ 
 public:
  Active_obj * head;
  Active_obj * tail;

  Ao_list() { head = NULL; tail = NULL; }
  void myinit()  { head = NULL; tail = NULL; }
  void myfini()  { head = NULL; tail = NULL; }
  void deletecontents() { if (head != NULL) delete head; head = NULL; tail = NULL; };
  ~Ao_list() { myfini(); }

  void append(Active_obj * obj);
};


class Request
{
 public:
  Obj_Type   kind;
  int	     noo_names;
  char	     reqform[40];
  char       names[100];
//char	     leadin[40];
  int	     error;
  char	     actform[40];
  char *     value;
  char *     href;

   Request() { reqform[0] = 0; actform[0] = 0; error = 0; value = NULL; href = NULL;
	       noo_names = 1;
	     };
   ~Request() { if (value != NULL) free(value); if (href != NULL) free(href); };

   void reinit() { if (value != NULL) free(value); if (href != NULL) free(href);
                   reqform[0] = 0; actform[0] = 0; error = 0; value = NULL; href = NULL;
	           noo_names = 1;
	         };

};

class Requests
{ 
  public:
	bool got_fixedtxt;
    char set_fixed_text[256 / 8];

  vector <Request> reqs;
  
  Requests() { got_fixedtxt = false; memset(set_fixed_text, 0, 256/8); };

  void add_req(const Request & rq);
  int find_name(const char * * resptr, const char * name, Obj_Type kind);
  void clear_results();
  Cc save(char * fname, int fix, const char * value, const char * href);
  void print();
};


char * string_skip(const char * s_);
char * string_get(char * & res, const char * s_);
char * get_name_end(const char * s);
char * get_token_dup(char * & res, const char * s);
char * get_ts_dup(char * & res, const char * s);
char * skip_ts(const char * s);
char * skip_to_lt(const char * s_);
char * get_to_lt_dup(char * & res, const char * s_);
char * strmatch(const char *t_, const char *s_);
char * strmatchword(const char *t_, const char *s_);
Cc get_active_objs(Ao_list & op, Requests & reqs, const char * buf);


extern char * cb_buf;
extern int    cb_max_size;
extern int    cb_size;


void * new_cb(int sz = MIN_PG_BUF_SZ);
void fin_cb();
int write_cb(char *in, int size, int nmemb, Char * type);


