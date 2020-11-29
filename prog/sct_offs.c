#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "build.h"


#define MAXLN 1024


typedef struct Field_eny_s
{ struct Field_eny_s *  next;
  char   	        c[MAXLN+1];
} Field_eny_t, * Field_eny;


Field_eny freelist = null;
Field_eny  field_list = null;


static char ipline[MAXLN+2];

Bool q_opt = false;

Bool diddef = false;


Bool  print_fields_called = false;

private void free_fields(fl)
     Field_eny fl;
{ while (fl != null)
  { Field_eny nxt = fl->next;
    free(fl);
    fl = nxt;
  }
}


private void print_fields()

{ Field_eny fl = field_list;
  
  while (fl != null)
  { if (q_opt)
      printf("   %s\n", &fl->c[0]);
    else
      printf("   printf(%c  %s	%%d%cn%c, offsetof((*sct), %s));\n", 
		'"', &fl->c[0], '\\', '"', &fl->c[0]);
    fl = fl->next;
  }
  
  free_fields(field_list);
  field_list = null;
  print_fields_called = true;
}


private void explain()

{ fprintf(stderr, "sct_offs filename*\n");
  fprintf(stderr, "   -q -- quick output\n");
}

void main(argc, argv)
        Int     argc;
        Char ** argv;
{ if (argc <= 1)
  { explain();
    exit(0);
  }
  
{ Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];
  
  for (; argsleft > 0; --argsleft)
  { if (argv_[0][0] == '-')
    { if      (argv_[0][1] == 'q')
        q_opt = true;
      else
      { explain();
        exit(0);
      }
    }
    else
    { Char * fn = argv_[0];
      FILE * chan = fopen(fn , "r");
      if (chan == null)
        fprintf(stderr, "Cannot open %s\n", fn);
      else
      { Int  struct_ct = 0;
        Bool in_struct = false;
	Quot cmt_st = 0;	    /* 1=> want opening *, 2=> want closing * */
				    /* 3=> want closing slash		      */
        Int brace_lvl = 0;
        Int brkt_lvl = 0;
        Int paren_lvl = 0;
	Bool need_struct_name = false;
	Int last_td_name = 1;
        Char struct_name[MAXLN];
        Char field_name[MAXLN];
        Bool ready_fields = false;

	if (not diddef)
	{ printf("#undef offsetof\n");
	  printf("#define offsetof(sct, fld) ((Int)&(sct).fld -(int)&(sct))\n");
	  diddef = true;
	}

	printf("#include %c%s%c\n\n", '"', fn, '"');
	printf("\n\nmain()\n\n");
      
        while (true)
	{ Char * last_word = null;
	  Char * ln = fgets(&ipline[0], MAXLN, chan);
	  if (ln == null)
	    break;
	    
	  for (;*ln != 0; ++ln)
	  { if (cmt_st == 0)
	    { if (*ln == '/')
	        cmt_st = 1;
	    }
	    else
	    { if      (cmt_st == 1)
	      { cmt_st = *ln != '*' ? 0 : 2;
	        if (cmt_st == 0 and ln > &ipline[0])
	          --ln;
	      }
	      else if (cmt_st == 2)
	        cmt_st = *ln != '*' ? 2 : 3;
	      else
	        cmt_st = *ln == '*' ? 3 :
			 *ln != '/' ? 3 : 0;
	      if (cmt_st != 0)
	        continue;
	    }
	    if (isspace(*ln))
	      ;
	    else if (*ln == '{')
	    { brace_lvl += 1;
	      if (brace_lvl == 1 and struct_name[0] != 0)
	      { if (q_opt)
	          printf(" STRUCT %s\n", struct_name);
	        else
	        { printf("{ struct %s * sct;\n", struct_name);
	          printf(" printf(%c STRUCT %s	%%d%cn%c, sizeof(*sct));\n",
				'"', struct_name, '\\', '"', struct_name);
	          struct_ct += 1;
	        }
	      }
	      else
	        struct_name[0] = 0;
	    }
	    else if (*ln == '}')
	    { brace_lvl -= 1;
	      if (brace_lvl == 0)
	        last_td_name = -1;
	      struct_name[0] = 0;
	    }
	    else if (*ln == '[')
	      brkt_lvl += 1;
	    else if (*ln == ']')
	      brkt_lvl -= 1;
	    else if (*ln == '(' or *ln == ')')
	    { paren_lvl += 2 * (*ln == '(') - 1;
	      if (brkt_lvl == 0)
	        field_name[0] = 0;
	    }
	    else if (isalpha(*ln) or *ln == '_')
	    { Char * ln_;
	      for (ln_ = ln; isalpha(*ln_) or isdigit(*ln_) or *ln_=='_'; ++ln_)
	        ;
	      
	    { Char sch = *ln_;
	      *ln_ = 0;
	      if      (strcmp(ln, "struct") == 0)
		need_struct_name = true;
	      else if (need_struct_name)
	      { strcpy(&struct_name[0], ln);
	        need_struct_name = false;
	      }
	      else if (last_td_name == 0 and field_list != null)
	      { if (q_opt)
	          printf("Typedef %s\n", ln);
	        else
	        { printf("{ %s * sct;\n", ln);
	          printf(" printf(%c typedef %s	%%d%cn%c, sizeof(*sct));\n",
				'"', ln, '\\', '"');
	          struct_ct += 1;
	        }
	        print_fields();
	        last_td_name = 1;
	      }
	      else if (struct_name[0] != 0 and
		       brkt_lvl == 0 and paren_lvl == 0)
	      { strcpy(&field_name[0], ln);
	      /*printf("Preparing %s %s ", struct_name, ln);*/
	      }
	      
	      *ln_ = sch;
	      ln = &ln_[-1];
	    }}
	    else if (field_name[0] != 0 and
		     struct_name[0] != 0)
	    { if ((*ln == ',' or *ln == ';') and
		     brkt_lvl == 0 and paren_lvl == 0)
	      { Field_eny fl = field_list;
	        Field_eny eny = (Field_eny)malloc(sizeof(Field_eny_t));
	        eny->next = null;
		strcpy(&eny->c[0], field_name);
		if (fl == null)			/* append to field_list */
		  field_list = eny;
		else
		{ while (fl->next != null)
		    fl = fl->next;
		  fl->next = eny;
		}
	      }

	      if (brkt_lvl == 0)
	        field_name[0] = 0;
	    }
	    if (last_td_name == 0)
	      print_fields();
	    last_td_name += 1;
	  }
	  if (cmt_st == 1 or cmt_st == 3)
	    cmt_st = 2;
	}      
        
        printf("\n");
        while (--struct_ct >= 0)
          printf("}");
        printf("\n");
        
        fclose(chan);
      }
    }

    ++argv_;
  }

  if (not print_fields_called)
  { printf("{ printf(%cNo Structures Found%cn%c);\n}\n", '"', '\\', '"');
  }
}}
