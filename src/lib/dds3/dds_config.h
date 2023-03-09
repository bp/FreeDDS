#ifndef __DDS_CONFIG_H
#define __DDS_CONFIG_H

#include <stddef.h>
#include <sys/types.h>

/*
RCS-t=DDS, private "config" (compile options and tuning)

$Id: dds_config.h 39 2009-01-13 23:00:15Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#undef  DDS_VALIDATE_PRINTFC
/* Turnoff validation, it presents problems with large # of sliced */
/* data filenames when writing "data=" in the OpenNameMake         */
/* #define DDS_VALIDATE_PRINTFC  1                                 */

#define DDS_VALIDATE_SCANFC  1   /* requires vsscanf (non-standard) */
#undef  DDS_VALIDATE_SCANFC

#undef  DDS_UNCOMPILE_CHECK
#define DDS_UNCOMPILE_CHECK  1   /* run time ddsUncompileFmt */

#undef  DDS_HEAP_CHECK
#define DDS_HEAP_CHECK  1        /* run time malloc overrun check */

#undef  DDS_SAFE_API
#define DDS_SAFE_API  1          /* API firewall checking */

#undef  FAST_INIT
#define FAST_INIT 1  /* fast initialization (avoid slow automounters) */

#define DDS_STRTO_LLONG strtol
#define DDS_STRTO_ULLONG strtoul
#define DDS_STRTO_LDOUBLE strtod

#define DDS_BAD_PTR   ((void*)-1)

/*********************************/
/* typedef and enumerated values */
/*********************************/

typedef enum DDS_CONFIG
{  /* miscellaneous integer constants, non-preprocessor and debugable */
   DDS_KB = 1024,             /* bytes per kilobyte */
   DDS_MB = 1024*1024,        /* bytes per megabyte */
   DDS_GB = 1024*1024*1024,   /* bytes per gigabyte */

   DDS_OK = 0,                /* api good return status */
   DDS_BAD = EOF,             /* api bad  return status */
   DDS_DENT = 2,              /* dump indententation */

   DDS_MOUNT_SLEEP = 10,      /* sleep time in seconds */
   DDS_MOUNT_PROMPT = 12,     /* initial prompt/sleep ratio */
   DDS_MOUNT_DAMPER = 4,      /* prompt/sleep scale factor */

   DDS_STR_MIN = 32,          /* min string size & modulo */
   DDS_STACK_MIN = 1024,      /* min stack size & modulo */
   DDS_PAGE_MIN = 1024,       /* min page size & modulo */

   DDS_DUMP_LIMIT = 1000,     /* maximum number of print elements */
   DDS_DUMP_QUE = 1024,       /* initial dump preamble size */
   DDS_DUMP_MAX = 256,        /* maximum preamble component */
   DDS_DUMP_WIDTH = 6,        /* minimum width of dump column */

   DDS_DEBUG_INIT   = 1 << 30,/* cdds_init automatic call */
   DDS_DBG_API = (DDS_DBG_CALL | DDS_DBG_TRACE | DDS_DEBUG_INIT),

   DDS_MSG_MAX = 8192,        /* max message phrase length */
   DDS_FTN_LINE = 120,        /* max fortran message line length */

   DDS_SLICE_LINE = 64,       /* max slice name line length */

   DDS_UNCOMP_LINE = 70,      /* max uncompile line length */
   DDS_UNCOMP_DENT =  3,      /* uncompile indentation length */

   DDS_SEEK_CHUNK = 2*DDS_MB, /* nominal bytes per read (seek emulation) */

   DDS_DEFN_NAME_MAX = 63,    /* nominal defn name max size */
   DDS_HUGE_IDENT = 2048,     /* huge identifer size */

   DDS_EOD_CHAR = '\004',     /* end-of-dict special character. */
   DDS_EOD_MODULO = 32,       /* end-of-dict modulo constraint. */
                              /*    Dictionary and binary data delimiter. */

   DDS_NON_CHAR = '\275',     /* invalid character */
   DDS_ASCII_ALERT = '\007',  /* ascii '\a' (bell/alert character) */

   DDS_HOST_BLANK = ' ',      /* host blank */
   DDS_HOST_t = '~',          /* host '~' */
   DDS_HOST_C = 'C',          /* host 'C' (uppercase) */
   DDS_HOST_c = 'c',          /* host 'c' (lowercase) */

   DDS_ASCII_BLANK = '\040',  /* ascii blank */
   DDS_ASCII_t = '\176',      /* ascii '~' */
   DDS_ASCII_C = '\103',      /* ascii 'C' (uppercase) */
   DDS_ASCII_c = '\143',      /* ascii 'c' (lowercase) */

   DDS_EBCDIC_BLANK = '\100', /* ebcdic blank */
   DDS_EBCDIC_t = '\241',     /* ebcdic '~' */
   DDS_EBCDIC_C = '\303',     /* ebcdic 'C' (uppercase) */
   DDS_EBCDIC_c = '\203'      /* ebcdic 'c' (lowercase) */

} DDS_CONFIG;

/*************************************************************************/
/* TYPE SELECTION GUIDE:                                                 */
/*                                                                       */
/* "size_t" must be avoided if the value may be formatted because there  */
/*    is not a portable conversion specifier for scan and print.         */
/*    "size_t" can NOT be used for values created by the DDS compiler.   */
/*                                                                       */
/* "long double" and "long long" should be avoided because some          */
/*    compilers (SGI IRIX32 and Sun4 respectively) don't support them.   */
/*    HOST_LDOUBLE_T and HOST_LLONG_T provides a portable alternative,   */
/*    iff the value does NOT have to be formatted by scan or print.      */
/*                                                                       */
/* HOST_INT6_T is required for byte offsets and sizes within files in    */
/*    order to support files sizes greater than 2**31.                   */
/*                                                                       */
/* "long" is used for byte offsets and sizes within memory               */
/*    that may be associated with arrays created by the DDS compiler.    */
/*    This includes struct and unions since they may contain arrays.     */
/*                                                                       */
/* "int" is used for most other variables.                               */
/*    "short" and "char" are used for "small" values iff storage         */
/*    overhead is more important than execution efficiency.              */
/*************************************************************************/


#endif /* __DDS_CONFIG_H */
