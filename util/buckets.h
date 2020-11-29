#define BUCKET_SZ   50


typedef struct Bucket
{ struct Bucket *    next;
  struct Bucket *    prev;
  Char               c[BUCKET_SZ];
} Bucket_t, *Bucket;		    /* C++ subtypes would be better for this */

typedef struct Nullbucket
{ struct Bucket *    next;
  struct Bucket *    prev;
} Nullbucket_t, *Nullbucket;



extern Cc bu_init();
extern void bu_to_head __((Nullbucket, Bucket));
extern void bu_new __((Nullbucket_t    *));
extern void bu_to_head __((Nullbucket_t    *, Bucket));

extern void bu_to_tail __((Nullbucket_t    *, Bucket));
extern Cc bu_from_head __((Bucket          *, Nullbucket_t    *));

extern Cc bu_from_tail __((Bucket, Nullbucket_t    *));

extern Cc bu_from_list __((Bucket, Nullbucket_t   *));
extern Bucket  bu_alloc();

extern Cc bu_free __((Bucket));

