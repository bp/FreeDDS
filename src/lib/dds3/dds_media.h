#ifndef __DDS_MEDIA_H
#define __DDS_MEDIA_H
 
/*
RCS-t=DDS, storage media "utility" declarations
 
$Id: dds_media.h 39 2009-01-13 23:00:15Z user $
*/
 
/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <sys/types.h>  /* dev_t */
 
/*********************************/
/* typedef and enumerated values */
/*********************************/

typedef enum DDS_MEDIA_MOUNT
{  /* media mount scheme */
   DDS_MEDIA_FILE,          /* single file */
   DDS_MEDIA_PATH,          /* multiple file/path */
   DDS_MEDIA_SLOT,          /* stacker slot number */
   DDS_MEDIA_LABEL,         /* external labels, no internal check */
   DDS_MEDIA_VOLSER,        /* check internal volume serial number */
   DDS_MEDIA_DSN            /* check internal dataset name */
} DDS_MEDIA_MOUNT;
 
typedef enum DDS_MEDIA_GENUS
{  /* media genus */
   DDS_MEDIA_ERR,               /* fstat/stat error (not accessible) */
   DDS_MEDIA_NOT,               /* fstat/stat not recognized */
   DDS_MEDIA_TTY,               /* terminal */
   DDS_MEDIA_CHR,               /* character special device (tape) */
   DDS_MEDIA_BLK,               /* block special device */
   DDS_MEDIA_REG,               /* regular file */
   DDS_MEDIA_DIR,               /* directory */
   DDS_MEDIA_FIFO,              /* pipe or socket */
   DDS_MEDIA_NULL,              /* /dev/null or memory */
   DDS_MEDIA_GENUS_MAX
} DDS_MEDIA_GENUS;
 
typedef enum DDS_MEDIA_STATE
{  /* bit flags defining media state */
   DDS_MEDIA_OPENR     = 1 <<  0,       /* opened for reading */
   DDS_MEDIA_OPENW     = 1 <<  1,       /* opened for writing */
   DDS_MEDIA_OPENA     = 1 <<  2,       /* writes append to end */

   DDS_MEDIA_OPENMODE  = (DDS_MEDIA_OPENR | DDS_MEDIA_OPENW | DDS_MEDIA_OPENA),

   DDS_MEDIA_SWAP      = 1 <<  3,       /* multi-media processing */
   DDS_MEDIA_UNLOAD    = 1 <<  4,       /* unload media when closed */
   DDS_MEDIA_NOLOAD    = 1 <<  5,       /* inhibit multiple media. */
                                        /*    Required when accessing */
                                        /*    stdin:, stdout: or fdN: */
                                        /*    and CYCLEFD is set. */
   DDS_MEDIA_BUFFER    = 1 <<  9,       /* buffer media */
   DDS_MEDIA_DICT1     = 1 << 11,       /* dictionary (primary) */
   DDS_MEDIA_DICT2     = 1 << 12        /* dictionary (secondary) */
} DDS_MEDIA_STATE;
 
typedef struct DDS_MEDIA
{  /* media description (one per GFILE, Unix file descriptor) */
   DDS_MEDIA_STATE state;       /* media state */

   GFILE *fp;                   /* real GFILE pointer */
   DDS_MEDIA_GENUS genus;       /* media genus (TAPE, FILE, FIFO, ...) */
                                /*    If sliced, the genus of each slice */
                                /*    (and master) must be the same. */
   const char *name;            /* logical name (filename, tmpXXX:, fd#:) */
                                /*    May be different than real name, */
                                /*    when associated with a mneumonic. */
                                /*    Real name is embedded in the title. */
   DDS_HASH hash;               /* logical name hash. */
   DDS_MEDIA_MOUNT mount;       /* PATH, SLOT, LABEL, VOLSER, DSN */
   const char *label;           /* label name */
   DDS_STRING title;            /* printable caption for media.  Example... */
                                /*    "file <real_name>" */
                                /*    "file <mneumonic> <real_name>" */
                                /*    "file fd<#>: */
                                /*    "file std{in|out}: */
                                /*    "tape <real_name> <#> <label>" */
                                /*    "tape fd<#>: */
                                /*    "tape std{in|out}: */
                                /*    "pipe <real_name>" */
                                /*    "pipe fd<#>: */
                                /*    "pipe std{in|out}: */
 
   struct DDS_DICT *dict;       /* dictionary cross reference */
   struct DDS_BIN *bin;         /* binary data cross reference */
   struct DDS_MEDIA *left;      /* double linked list of ALL media. */
   struct DDS_MEDIA *right;     /*    For overall media management. */

   struct DDS_MEDIA *prev;      /* double linked list of logical media. */
   struct DDS_MEDIA *next;      /*    For multi tape support. */

   int file_skip;               /* EOF skip count (applied to each media). */
                                /*    Used to skip internal labels, etc. */
   int file_count;              /* File number on current media (tape). */
                                /*    Zero if media is regular disk file. */
                                /*    Set to one, when media is mounted. */
                                /*    Incremented after each EOF. */
 
   HOST_LLONG_T first_trace;    /* trace offset to first trace in file */
                                /*    Zero on first media only. */
   HOST_LLONG_T final_trace;    /* trace offset to last trace + 1 */
                                /*    Equal to first_trace on next media. */
                                /*    missing traces are not counted. */
                                /*    Related to DDS_BIN LineTrc counter. */
 
   HOST_INT6_T data_bias;       /* byte offset to "line hdr" on media. */
                                /*    Byte size of dictionary preamble. */
   HOST_INT6_T trace_bias;      /* byte offset to "traces" on media. */
                                /*    Byte size of dict and line header. */
 
   long ExaByteFix;     /* kludge for 2 gigabyte limit on tape size. */
                        /*    Used to reset device driver near the limit. */

   double balance;      /* storage capacity remaining {sectors | inches} */
   double toll;         /* comsumption rate, {sectors | inches} per byte */
   float gap;           /* inner record gap {sectors | inches} */

   float rate;          /* bytes per sec transfer rate */
   int density;         /* bytes per {sector or inch} */
   float duty;          /* duty factor < 1. (with defect rate) */
   float capacity;      /* storage capacity, bytes per media. */
                        /*    Approximately (density * quantity) */
 
   int sectors;         /* sectors per track */
   int tracks;          /* tracks per cylinder */
   int cylinders;       /* cylinders per volume */
                        /*    quantity = sectors * tracks * cylinders */

   int passes;          /* passes over tape to EOT */
   int length;          /* tape length in feet */
                        /*    quantity = passes * length * INCHES_PER_FOOT */
 
   int min_block;       /* minimum block size, in bytes */
   int max_block;       /* maximum block size, in bytes */
   int opt_block;       /* optimum block size, in bytes */
 
} DDS_MEDIA;

/* Currently only supporting USP, SEGY & GENERIC */
/* (GENERIC includes asp, which in itself contains all the cube formats and sep )*/
typedef enum DDS_MEDIA_FMT
{  /* media formats */
   DDS_FMT_UNKNOWN,         /* unknown */
   DDS_FMT_DICT,            /* dictionary *** DEPRECATED***  */
   DDS_FMT_GENERIC,         /* generic binary */
   DDS_FMT_USP,             /* Amoco usp/sis */
   DDS_FMT_SEGY,            /* SEGY */
   DDS_FMT_DISCO,           /* Cogniseis Disco */
   DDS_FMT_GEOQUEST,        /* Geoquest */
   DDS_FMT_LANDMARK,        /* Landmark */
   DDS_FMT_SU,              /* Delft SU Format */
   DDS_FMT_SEGY1,           /* SEGY Rev. 1 */
   DDS_MEDIA_FMT_MAX        /* keyword count */
} DDS_MEDIA_FMT;


typedef struct DDS_STAT_ID
{  /* unique file id, based upon stat system call */
   dev_t st_dev;
   ino_t st_ino;
} DDS_STAT_ID;

typedef enum DDS_AUTO_FLAG
{  /* bit flags returned by automatic format detection */
      DDS_AUTO_GREEN_PREFIX = 1 <<  0,
      DDS_AUTO_GREEN_SUFFIX = 1 <<  1,
      DDS_AUTO_ASCII        = 1 <<  2,
      DDS_AUTO_EBCDIC       = 1 <<  3,
      DDS_AUTO_IBM          = 1 <<  4,
      DDS_AUTO_IEEE         = 1 <<  5,
      DDS_AUTO_OSLABEL      = 1 << 26,
      DDS_AUTO_BLOCKED      = 1 << 27,
      DDS_AUTO_CRAM         = 1 << 28,
      DDS_AUTO_ERROR        = 1 << 29,
      DDS_AUTO_UNKNOWN      = 1 << 30
} DDS_AUTO_FLAG;


typedef struct DDS_AUTO_FMT
{  /* Results of automatic format detection on binary stream */
   DDS_AUTO_FLAG flag;
   DDS_MEDIA_FMT fmt;      /* GENERIC, SEP, USP, SEGY, DISCO, ... */
   DDS_TYPE_GENUS genus;   /* Sample type (integer, float, complex, ...) */
   int prec;               /* Sample precision */
} DDS_AUTO_FMT;
 
/*****************************************/
/* extern global variables and constants */
/*****************************************/

extern DDS_MEDIA *dds_media_list;       /* media access list (NULL == none) */

extern const char * const dds_fmt_name[DDS_MEDIA_FMT_MAX];

extern const char * const dds_media_genus[DDS_MEDIA_GENUS_MAX];

/***********************/
/* function prototypes */
/***********************/

int ddsAutoFmt(
   DDS_AUTO_FMT *AutoFmt,
   GFILE        *fp)
;
 
DDS_MEDIA * ddsMediaFind(
   const char *name,
   int fd)
;
DDS_MEDIA * ddsMediaNew(
   struct DDS_DICT *dict,
   struct DDS_BIN *bin,
   DDS_MEDIA_STATE state,
   const char *name,
   GFILE *fp)
;
void ddsMediaChain(
   DDS_MEDIA *media,
   DDS_MEDIA *chain)
;
int ddsMediaDelete(
   DDS_MEDIA *media)
;
void ddsMediaScan(
   const char *alias,
   DDS_MEDIA *media)
;
int ddsMediaBlock(
   DDS_MEDIA *media,
   int block)
;
int ddsMediaFirst(
   DDS_MEDIA *media,
   const char *mode,
   const char *dict)
;
int ddsMediaLoad(
   DDS_MEDIA *media,
   const char *mode)
;
int ddsMediaUnload(
   DDS_MEDIA *media)
;


int ddsStatID(
   DDS_STAT_ID *stat_id,
   const char *name)
;
int ddsFStatID(
   DDS_STAT_ID *stat_id,
   int fd)
;

DDS_MEDIA_GENUS ddsStatGenus(
   const char *name)
;
DDS_MEDIA_GENUS ddsFstatGenus(
   int fd)
;
off_t ddsFstatSize(
   int fd)
;

const char * ddsPathClean(
   const char *path)
;
const char * ddsPathFast(
   const char *path)
;


int ddsRevealName(
   int fd,
   char* string,
   int maxlen);


DDS_MEDIA * ddsClash(
   const char *preamble,
   const char *name,
   int fd)
;
 

#endif /* __DDS_MEDIA_H */
