#include   "build.h"
#include   "buckets.h"

			/* MODULE buckets */
			
/* This module manages NOO_BUCKETS buckets of size BUCKET_SZ */
/* and doubly linked lists of them                           */

/* Initially all the buckets are on the free list */
/* buckets may be added to and removed from the free list */


#define c_malloc chk_malloc
#define c_free   chk_free

#define NOO_BUCKETS 100


Nullbucket_t head_free;		/* field c is not used */




public Cc bu_init()

{ bu_new(&head_free);
  
{ Int i;
  for (i = NOO_BUCKETS; i > 0; --i)
  { Bucket bu = (Bucket)c_malloc(sizeof(Bucket_t));
    if (bu == null)
      return ENOSPACE;
    bu_to_head(&head_free, bu);
  }
}}

public void bu_new(listhead)
	Nullbucket_t    * listhead;
{ listhead->next = (Bucket)listhead;
  listhead->prev = (Bucket)listhead;
}



public void bu_to_head(listhead, bkt)
	Nullbucket_t    * listhead;
	Bucket            bkt;
{ bkt->next = listhead->next;
  bkt->prev = (Bucket)listhead;

  if (listhead->next == (Bucket)listhead)
    listhead->prev = bkt;

  listhead->next = bkt;
}



public void bu_to_tail(listhead, bkt)
	Nullbucket_t    * listhead;
	Bucket            bkt;
{ bkt->next = (Bucket)listhead;
  bkt->prev = listhead->prev;

  if (listhead->next == (Bucket)listhead)
    listhead->next = bkt;

  listhead->prev = bkt;
}

public Cc bu_from_head(bkt_ref, listhead)
	Bucket          * bkt_ref;
	Nullbucket_t    * listhead;
{ if (listhead->next == (Bucket)listhead)
    return NOT_FOUND;

  *bkt_ref = listhead->next;
  listhead->next = listhead->next->next;
  if (listhead->next == (Bucket)listhead)
    listhead->prev = (Bucket)listhead;
}



public Cc bu_from_tail(bkt_ref, listhead)
	Bucket          * bkt_ref;
	Nullbucket_t    * listhead;
{ if (listhead->next == (Bucket)listhead)
    return NOT_FOUND;

  *bkt_ref = listhead->prev;
  listhead->prev = listhead->prev->prev;
  if (listhead->prev == (Bucket)listhead)
    listhead->next = (Bucket)listhead;

  return OK;
}



public Cc bu_from_list(listhead, bkt)
	Bucket           bkt;
	Nullbucket_t   * listhead;
{ Bucket this;

  for (this = listhead->next; this != (Bucket)listhead; this = this->next)
    if (this == bkt)
    { bkt->prev->next = bkt->next;
      bkt->next->prev = bkt->prev;
      return OK;
    }
    
  return NOT_FOUND;
}

public Bucket  bu_alloc()

{ Bucket res;
  Cc cc = bu_from_head(&res, head_free);
  return cc == NOT_FOUND ? null : res;  
}




public Cc bu_free(bkt)
	Bucket  bkt;
{
#if DEBUG > 0
  Cc cc = bu_from_list(&head_free, bkt);
  if (cc != OK)
  { i_log(cc, "Attempt to free unallocated bucket");
    return cc;
  }
#endif
  bkt->next->prev = bkt->prev;
  bkt->prev->next = bkt->next;
  return OK;
}


