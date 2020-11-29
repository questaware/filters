#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include "build.h"
#include "list.h"




public void ppend(lh, cell)
	Listhead_t    * lh;
	List            cell;
{ cell->next = lh->head;

  if (cell->next == null)
    lh->tail = cell;
  lh->head = cell;
}



public void ppend_obj(lh, obj)
	Listhead_t    * lh;
	Char *          obj;
{ List cell  = mkcell();
  cell->c    = (void*)obj;

  cell->next = lh->head;
  if (cell->next == null)
    lh->tail = cell;
  lh->head = cell;
}



public void append(lh, cell)
	Listhead_t    * lh;
	List            cell;
{ cell->next = null;
 
  if (lh->head == null)
  { lh->head = cell;
    lh->tail = cell;
  }
  else
  { lh->tail->next = cell;
    lh->tail = cell;
  }
}


public void append_obj(lh, obj)
	Listhead_t    * lh;
	Char *          obj;
{ List cell  = mkcell();
  cell->c    = (void*)obj;
  cell->next = null;
 
  if (lh->head == null)
  { lh->head = cell;
    lh->tail = cell;
  }
  else
  { lh->tail->next = cell;
    lh->tail = cell;
  }
}



public List from_head(lh)
	Listhead  lh;
{ if (lh == null)
    return null;
{ List res = lh->head;
  lh->head = res == null or res == lh->tail ? null
					    : res->next;
  return res;
}}



public Listhead mklist()

{ Listhead lh = (Listhead)malloc(sizeof(Listhead_t));
  lh->head = null;
  return lh;
}



public List mkcell()

{ List cell = (List)malloc(sizeof(List_t));
  cell->next = null;
  return cell;
}
