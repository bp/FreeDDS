#ifndef __DDS_TABLE_H
#define __DDS_TABLE_H
 
/*
RCS-t=DDS, hash table "utility" declarations
 
$Id: dds_table.h 16 2009-01-06 21:52:47Z user $
*/

/*********************************/
/* typedef and enumerated values */
/*********************************/

/***********************************************************************/
/**  Dictionaries, definitions, and compilers all use "hash" tables.  **/
/**  Support functions operate on DDS_TABLE, DDS_ITEM and DDS_PAGE.   **/
/**  Functions are named "ddsTable*.c" (Init, Term, Append, Lookup,   **/
/**  Unhash, Delete, and Rehash).                                     **/
/**  Data in a hash page may be named, anonymous, or un-linked.       **/
/**     named: definitions, dictionaries, typedefs, variables.        **/
/**        Data is found using hash table lookup (fast, extendable).  **/
/**     anonymous: nested blocks, array types, control info.          **/
/**        Data is found by linearly searching double linked lists.   **/
/**        Anonymous lists are delimited by named hash table items,   **/
/**        They share the same list and chronological sequence.       **/
/**     un-linked: strings, constants, variables, arrays.             **/
/**        Arbitary data may be pushed onto the same stack used by    **/
/**        the hash table to manage named and un-named items.         **/
/**        Data is found using extended attributes on table entries   **/
/**        and references from outside the table structure.           **/
/**  When practical, all related data is kept in the same table.      **/
/**  Page management is more efficient than general heap support.     **/
/***********************************************************************/
 
 
/*********************************************************************/
/* Each entry in a hash table begins with a DDS_ITEM_CLASS struct.   */
/* It keeps the basic description for an individual entry.           */
/* Extended entries must inherit DDS_ITEM_CLASS (first member).     */
/*                                                                   */
/* The "named" entries can be used to delimit an anonymous list.     */
/*                                                                   */
/* Entries may be anonymous, i.e. name is zero length string.       */
/* Anonymous entries do *not* appear in the hash table itself.      */
/* They *do* appear in the double linked list ("older", "newer")    */
/* of all entries, along with "named" hash table entries.           */
/* The "hash" and "chain" values may be overloaded, i.e. used for    */
/* other purposes if the the item is anonymous.                      */
/*********************************************************************/
 
 
/*** "poor mans" inheritance scheme ***/
#define DDS_ITEM_CLASS       /*******************************************/ \
   DDS_HASH hash;            /* Hash value of item name                 */ \
                             /*    Names are compared only if their     */ \
                             /*    hash values match (fast, plausible). */ \
                             /*    Tables can be quickly rehashed,      */ \
                             /*    using pre-computed hash values.      */ \
                             /*    Anonymous list items may overload    */ \
                             /*    the "hash" member, because table     */ \
                             /*    utilities will ignore the value.     */ \
   const char *name;         /* String pointer, item name               */ \
                             /*    If anonymous, name points to a zero  */ \
                             /*    length string (name is never NULL).  */ \
   struct DDS_ITEM *chain;   /* hash table overflow buckets.            */ \
                             /*    NULL, if last entry on bucket chain. */ \
                             /*    Non-zero, if an "older" item hashes  */ \
                             /*    to the same bucket (overflow).       */ \
                             /*    Anonymous list items may overload    */ \
                             /*    the usage of "chain", because table  */ \
                             /*    utilities will ignore the value.     */ \
   struct DDS_ITEM *older;   /* older (left) item.                      */ \
                             /*    Chronological sequence of items.     */ \
                             /*    NULL, if "oldest" entry in table.    */ \
   struct DDS_ITEM *newer;   /* newer (right) item.                     */ \
                             /*    Chronological sequence of items.     */ \
                             /*    NULL, if "newest" entry in table.    */ \
                             /*******************************************/
 
typedef struct DDS_ITEM
{  /* generic item descriptor for hash table entries */
   DDS_ITEM_CLASS            /* hash table item */
} DDS_ITEM;
 
 
typedef struct DDS_PAGE
{  /* page descriptor for hash table items */
   size_t top;               /* index of next available character */
   size_t max;               /* malloc maximum, i.e. buffer[max] */
   char *ptr;                /* pointer to start of buffer. */
                             /*    The beginning of each buffer */
                             /*    contains the DDS_PAGE descriptor */
                             /*    for the previous page, if any. */
} DDS_PAGE;
 
 
typedef struct DDS_TABLE
{  /* hash table descriptor (see DDS_STACK and DDS_ITEM) */
   /**********************************************************************/
   /* A hash table is summarized by a DDS_TABLE structure.               */
   /* Storage for DDS_ITEMs in DDS_TABLE is described by DDS_PAGE.   */
   /* Pages also contain name strings and other non-DDS_ITEM data. */
   /* Data can be appended to the active DDS_PAGE in DDS_TABLE. */
   /* When the active page overflows, a new page is allocated. */
   /* Previous pages are chained together via their initial entry. */
   /* Only the last DDS_ITEM in the active page can be deleted. */
   /**********************************************************************/
   DDS_ITEM **chain;         /* bucket array pointer (bucket[tbl.size]) */
   DDS_ITEM *oldest;         /* oldest item (item->older == NULL) */
                             /*    NULL, if chain is empty. */
   DDS_ITEM *newest;         /* newest item (item->newer == NULL) */
                             /*    NULL, if chain is empty. */
   int size;                 /* number of buckets in hash table (prime) */
   int load;                 /* number of buckets with non-zero values */
   int count;                /* number of *named* table entries. */
                             /*    Anonymous entries are *not* counted. */
   int unique;               /* number of unique *named* table entries. */
                             /*    Note: count >= unique >= load */
   double alpha;             /* rehash threshold, < (count / hash_load) */
                             /*    Average number of items per bucket. */
                             /*    Controls automatic rehashing. */
   double beta;              /* rehash new size >= (count / beta). */
                             /*    Most optimistic alpha after rehash. */
                             /*    Controls table growth when rehashing. */
   DDS_PAGE page;            /* strings, descriptors, constants, etc. */
                             /*    Active page for appending new items. */
                             /*    Pages contain items (named, anonymous) */
                             /*    and un-linked data for "things". */
} DDS_TABLE;
 
 
/******************************************************************/
/* Most hash table entries are extensions of the DDS_ITEM, */
/* i.e. DDS_ITEM_CLASS is the first member of their structure.      */
/* Additional attributes are appended to the structure as needed. */
/* Examples include DDS_DEFN, DDS_DICT, and DDS_KEYWORD.        */
/******************************************************************/


/*****************************************/
/* extern global variables and constants */
/*****************************************/

/* Initial size of hash tables */
extern const int    KEYS_HASH_SIZE;
extern const int    SYM_HASH_SIZE;
extern const int    DICT_HASH_SIZE;
extern const int    DEFN_HASH_SIZE;
extern const int    STOP_HASH_SIZE;
 
/* ALPHA, rehash threshold */
extern const double KEYS_HASH_ALPHA;
extern const double SYM_HASH_ALPHA;
extern const double DICT_HASH_ALPHA;
extern const double DEFN_HASH_ALPHA;
extern const double STOP_HASH_ALPHA;
 
/* BETA, items per bucket */
extern const double KEYS_HASH_BETA;
extern const double SYM_HASH_BETA;
extern const double DICT_HASH_BETA;
extern const double DEFN_HASH_BETA;
extern const double STOP_HASH_BETA;

 
/* zero length string for un-named symbols */
extern const char *dds_anonymous;

/***********************/
/* function prototypes */
/***********************/

DDS_ITEM * ddsTableUnhash(
   DDS_TABLE *table,
   DDS_ITEM *item)
;
size_t ddsTableInit(
   DDS_TABLE *table,
   size_t size,
   size_t min,
   double alpha,
   double beta)
;
void * ddsTablePush(
   DDS_TABLE *table,
   size_t size,
   size_t align)
;
void * ddsTablePushZero(
   DDS_TABLE *table,
   size_t size,
   size_t align)
;
const char * ddsTablePushChar(
   DDS_TABLE *table,
   const char *start,
   char value)
;
const char * ddsTablePushStr(
   DDS_TABLE *table,
   const char *str)
;
DDS_ITEM * ddsTablePushItem(
   DDS_TABLE *table,
   const char *name,
   size_t item_size)
;
DDS_ITEM * ddsTableLookup(
   DDS_TABLE *table,
   const char *name)
;
size_t ddsTableRehash(
   DDS_TABLE *table)
;
void ddsTableTerm(
   DDS_TABLE *table)
;

#endif /* __DDS_TABLE_H */
