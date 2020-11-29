typedef struct List_s
{ struct List_s  * next;
  void *           c;
} List_t, *List;
				/* List_t is a list cell.
				   List cells can be bigger than List_t.
				*/
typedef struct Listhead_s
{ List head;
  List tail;
} Listhead_t, *Listhead;


extern void ppend __((Listhead, List));
extern void append __((Listhead, List));
extern void ppend_obj __((Listhead, Char *));
extern void append_obj __((Listhead, Char *));
extern List from_head __((Listhead));
extern Listhead mklist();
extern List mkcell();
