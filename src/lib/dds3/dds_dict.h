#ifndef __DDS_DICT_H
#define __DDS_DICT_H

/*
RCS-t=DDS, dictionary "utility" declarations

$Id: dds_dict.h 16 2009-01-06 21:52:47Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#if defined (__FreeBSD__ ) || defined ( linux )
   /* Define the KERNEL symbol for FreeBSD so that stdarg allows short types */
   #define KERNEL

   #if !(defined (sun) && !defined(__SVR4) && !defined(__GNUC__))
      #include <stdarg.h>
   #else   /* !(defined (sun) && !defined(__SVR4) && !defined(__GNUC__)) */
      #include <varargs.h>
   #endif  /* !(defined (sun) && !defined(__SVR4) && !defined(__GNUC__)) */

   #undef  KERNEL
#else
   #include <stdarg.h>
#endif

/* Initial size for dictionary buffer */
#define DICT_BUF_SIZE (2 * DDS_KB)

/* definition name for process sentry and history title. */
#define DDS_SENTRY "DDS_SENTRY"

/* special definition name, delimit dictionary from binary data */
#define EOD_DEFN_NAME "DDS:END_OF_DICT"
 
/* test characters for definition name syntax */
#define DEFN_SYNTAX(s) (isgraph(s) && s != '=' && s != '\\')
 

/*************************************************/
/* macro to load strings into current dictionary */
/*************************************************/
 
#define LDS(STR) ddsDictLoad(dict, load, NULL, STR, strlen(STR));
 
/**********************************************************/
/* macro to convert Fortran format string into a C string */
/**********************************************************/
 
#define FTN_FMT_TO_STR(FMT_STR, FMT_FTN)  \
   { if(! (FMT_STR).max) ddsStrSetMin(&(FMT_STR), "", FMT_NOM_LEN);  \
      ddsStrCpyF(&(FMT_STR), (FMT_FTN)); }

/*********************************/
/* typedef and enumerated values */
/*********************************/

typedef enum DDS_DICT_STATE
{  /* bit flags defining dictionary state */
   DDS_DICT_ACTIVE      = 1 << 0,       /* live hash table entry */
   DDS_DICT_READ_ONLY   = 1 << 1,       /* read only (never written) */
   DDS_DICT_FROZEN      = 1 << 2,       /* write mode terminated */
   DDS_DICT_LOAD        = 1 << 3,       /* read dictionary from stream */
   DDS_DICT_TRUNCATE    = 1 << 4,       /* truncate to zero length */
   DDS_DICT_SCAN        = 1 << 5,       /* selected for scanning */
   DDS_DICT_PRINT       = 1 << 6,       /* selected for printing */
   DDS_DICT_STR_BUF     = 1 << 7,       /* string buffer (don't free) */
   DDS_DICT_SHAM        = 1 << 9,       /* fake dict (binary detected) */
   DDS_DICT_MAP         = 1 << 10,      /* special source for map defns */
   DDS_DICT_DATA        = 1 << 30       /* binary data attached to dict */
} DDS_DICT_STATE;
 
typedef struct DDS_LOAD
{  /* dictionary load state */
   DDS_BOOLEAN escape;                  /* escape state variable */
   DDS_BOOLEAN quote;                   /* quote state variable */
   size_t black_size;                   /* chars in definition name */
   size_t white_size;                   /* chars between name and "=" */
   const char *name;                    /* recursive load name */
   struct DDS_LOAD *prev;               /* recursive load chain */
   size_t depth;                        /* recursive load depth */
} DDS_LOAD;
 
typedef struct DDS_DICT
{  /* dictionary descriptor (one per dictionary) */
   DDS_ITEM_CLASS            /* hash table item */
                             /*    (hash, name, chain, older, newer) */
   DDS_DICT_STATE state;     /* dictionary state */
   const char *alias;        /* logical alias ("in", "out", etc.) */
   struct DDS_MEDIA *media;  /* media descriptor (name, fd, etc.) */
   int error;                /* error register. */
   DDS_STRING buf;           /* dict buffer (image of i/o stream) */
   size_t buf_flush;         /* dict buffer flush index (next write) */
   struct DDS_DICT *right;   /* older dict in scan precedence. */
                             /*    NULL, if oldest dict. */
   struct DDS_DICT *left;    /* newer dict in scan precedence. */
                             /*    NULL, if newest dict. */
   DDS_TABLE defn;           /* definition hash table. */
                             /*    Each entry marks a definition. */
                             /*    The "newest" defn is anonymous. */
                             /*    It servers as the eof sentinel. */
   int line;                 /* new-line counter within dict buffer. */
                             /*    Incremented when '\n' is appended. */
   int black_line;           /* black-space, new-line counter. */
                             /*    Saved when black-space is seen. */
   DDS_LOAD load;            /* load state variable. */
                             /*    Used to catch new definitions as */
                             /*    the dictionary buffer is loaded. */
   DDS_STRING print_name;    /* name of last defn (prior to eof defn). */
                             /*    The value for this definition can */
                             /*    grow as characters are appended. */
   gpos_t data_position;     /* binary data stream position */
} DDS_DICT;
 
typedef struct DDS_DEFN
{  /* definition hash bucket (extended hash table entry) */
   DDS_ITEM_CLASS            /* hash table item */
                             /*    (hash, name, chain, older, newer) */
   size_t start;             /* start index within dictionary buffer. */
                             /*    buf[start] is the first character */
                             /*    of *this* definition (alias/name). */
                             /*    It's also the eof marker for the */
                             /*    previous defn, i.e. temporarily */
                             /*    replaced by a NUL character when */
                             /*    previous defn is selected by scan. */
   int alias;                /* alias size (non-zero if alias defn) */
                             /*    Zero, if "name = value". */
                             /*    One,  if "$name = value". */
                             /*    Six,  if "alias:name = value". */
   int line;                 /* starting line number within dictionary. */
                             /*    Used for diagnostics messages only. */
   int status;               /* status: 1=used. */
} DDS_DEFN;
 
typedef struct DDS_ALIAS
{  /* alias descriptor for definition name */
   DDS_STRING name;                   /* defn name */
   DDS_HASH hash;                     /* defn name hash */
   size_t size;                       /* defn name size */
   DDS_BOOLEAN alias;                 /* defn alias ($xx, alias:xx) */
   struct DDS_ALIAS *left;            /* alias left list */
   struct DDS_ALIAS *right;           /* alias right (or free) list */
} DDS_ALIAS;

/*****************************************/
/* extern global variables and constants */
/*****************************************/

extern DDS_TABLE dds_dict;            /* dictionary hash table */

extern DDS_DICT *dds_scan_first;      /* newest scan dict (NULL == none) */
extern DDS_DICT *dds_scan_last;       /* oldest scan dict (NULL == none) */

                                      /* current scan dict, defn, and value */
extern DDS_DICT *dds_scan_dict;       /* scan dict descriptor (NULL == none) */
extern const DDS_DEFN *dds_scan_defn; /* scan defn descriptor */
extern const char *dds_scan_name;     /* scan defn name */
extern size_t dds_scan_value_bias;    /* scan defn value bias */
extern DDS_TOKEN dds_scan_token;      /* scan token value and next char* */
extern char dds_scan_value_save;      /* scan defn value save char */
extern const DDS_DEFN *dds_scan_newer;/* scan defn newer defn descriptor */

                                      /* current print dict */
extern DDS_DICT *dds_print_first;     /* newest print dict (NULL == none) */
extern DDS_DICT *dds_print_last;      /* oldest print dict (NULL == none) */

extern DDS_DICT *dds_log_dict;        /* log   dict descriptor (NULL == none) */

extern DDS_ALIAS *dds_alias_free;     /* malloc alias list (NULL == none) */
extern DDS_ALIAS *dds_alias_done;     /* unused alias list (NULL == none) */
extern unsigned dds_alias_count;      /* alias count (malloc total) */


/***********************/
/* function prototypes */
/***********************/

void ddsParCheck(
   int check)
;
void ddsParTag(
   DDS_DEFN *defn,
   const DDS_DICT *dict)
;
const DDS_DEFN * ddsDefnFind(
   size_t *save_size,
   size_t *save_bias,
   DDS_ALIAS **save_list,
   const DDS_DICT *dict)
;
const DDS_DEFN * ddsDefnNewer(
   size_t *save_bias,
   DDS_ALIAS **save_list,
   const DDS_DICT *dict,
   const DDS_DEFN *defn)
;
DDS_DEFN * ddsDefnNew(
   DDS_DICT *dict,
   size_t start,
   size_t alias_size,
   const char *name)
;
 
 
int ddsHistory(
   DDS_DICT *dict_new,
   const DDS_DICT *dict_old)
;
 
 
int ddsScanfC(
   const char **value,
   va_list *ap,
   const char *fmt,
   const char *dict_name,
   const char *defn_name)
;
int ddsScanfF(
   const char **value,
   va_list *ap,
   va_list *ap_len,
   const char *fmt,
   const char *dict_name,
   const char *defn_name)
;
int ddsKey(
   const char *alias)
;
int ddsScanDone(
   void)
;
 
 
int ddsPrintfC(
   DDS_DICT *dict,
   va_list *ap,
   const char *fmt)
;
int ddsPrintfF(
   DDS_DICT *dict,
   va_list *ap,
   va_list *ap_len,
   const char *fmt)
;
int ddsPrintDone(
   void)
;
 
 
DDS_STRING * ddsAliasName3(
   DDS_STRING *str,
   const char *mode,
   const char *name1,
   const char *name2,
   const char *name3)
;
DDS_STRING * ddsAliasName2(
   DDS_STRING *str,
   const char *mode,
   const char *name1,
   const char *name2)
;


DDS_ALIAS * ddsAliasList(
   DDS_ALIAS *drop,
   const char *alias_list)
;
DDS_ALIAS * ddsAliasAdd(
   DDS_ALIAS *list,
   const char *name,
   size_t size)
;
DDS_ALIAS * ddsAliasDrop(
   DDS_ALIAS *alias)
;
void ddsAliasDone(
   DDS_ALIAS *list)
;


int ddsTitle(
   const char *title)
;
const char * ddsTmpDict(
   const char *clue)
;


const char * ddsDefnAxis(
   const char *alias,
   const DDS_DICT *dict)
;


const char * ddsFindDDSFile(
   const char *name,
   const char *hist,
   const char *default_name)
;


struct DDS_DICT * ddsDictAspFmt(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictAspMap(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictCmd(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
int ddsDictDelete(
   DDS_DICT *dict)
;
DDS_DICT * ddsDictDevice(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictEnv(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictFd(
   const char *name,
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictFile(
   const char *name,
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictInit(
   const char *name,
   const char *mode)
;
int ddsDictLoad(
   DDS_DICT *dict,
   DDS_LOAD *load,
   GFILE *fp,
   const char *str,
   size_t size)
;
int ddsParLoad(
   DDS_TOKEN *list,
   DDS_DICT *dict,
   DDS_LOAD *load) 
;
DDS_DICT * ddsDictNew(
   const char *name,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state,
   const char *media_name,
   GFILE *media_fp,
   char *buf_str,
   size_t buf_size)
;
DDS_DICT * ddsDictPar(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictSegyFmt(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictSegyMap(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictSegy1Fmt(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictSegy1Map(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictStdin(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictStdout(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictStderr(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictLog(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictTmp(
   const char *name,
   const char *mode,
   DDS_DICT_STATE dict_state)
;
DDS_DICT * ddsDictUspFmt(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
DDS_DICT * ddsDictUspMap(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
;
void ddsReadLog()
;
void ddsPrintLog(
   const char *alias,
   const char *value)
;
int ddsScanLog(
   const char *alias,
   const char **value)
;


#endif /* __DDS_DICT_H */
