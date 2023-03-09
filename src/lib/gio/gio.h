#ifndef __CGIO_H
#define __CGIO_H

/*
"C" Geoscience IO package.
Application Program Interface (visible to programmers).

$Id: gio.h 199 2010-11-04 17:14:10Z user $
*/

/********************************************************************

   These functions provide access to an i/o stream.
   Data can be copied between gio buffers and buffers provided
   by the caller (traditional i/o model).
   Data can also be referenced via pointers directly within gio buffers.
   This may eliminate redundant buffers and data copying.

   Function names, return values and formal arguments parallel stdio.
   The GFILE type is similar to, but not compatible with FILE.
   The names of functions which use GFILE are prefixed by "g", NOT "f".
   This will reduces the learning curve between gio and stdio.

   All symbols visible within the API, but not part of the application
   program interface, are prefixed by "GIO" or "gio", to avoid clashes.

   Reference: "The Alloc Stream Facility"
      A Redesign of Application-Level Stream I/O
      Orran Krieger and Michael Stumm, University of Toronto
      Ron Unrau, IBM Canada
      IEEE Computer, March 1994, Volume 27 Number 3
********************************************************************/

#include <stdio.h>
#include "chost.h"
#include "gio_config.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef enum
{  /* GIO buffer address constraints */
   GIO_CHAR            = HOST_CHAR_A,
   GIO_UCHAR           = HOST_UCHAR_A,
   GIO_SCHAR           = HOST_SCHAR_A,

   GIO_USHORT          = HOST_USHORT_A,
   GIO_UINT            = HOST_UINT_A,
   GIO_ULONG           = HOST_ULONG_A,
   GIO_ULLONG          = HOST_ULLONG_A,

   GIO_SHORT           = HOST_SHORT_A,
   GIO_INT             = HOST_INT_A,
   GIO_LONG            = HOST_LONG_A,
   GIO_LLONG           = HOST_LLONG_A,

   GIO_FLOAT           = HOST_FLOAT_A,
   GIO_DOUBLE          = HOST_DOUBLE_A,
   GIO_LDOUBLE         = HOST_LDOUBLE_A,

   GIO_PTR             = HOST_PTR_A,
   GIO_MALLOC          = HOST_MALLOC_A
} GIO_ALIGN;

typedef enum
{  /* gopenmax options */
   GIO_OPEN_MAX_SET,
   GIO_OPEN_MAX_INCR,
   GIO_OPEN_MAX_RESET,
   GIO_OPEN_MAX_GET
} GIO_OPEN_MAX_OPTION;

#ifdef  __cplusplus
}
#endif

#include "gio_types.h"

#ifdef  __cplusplus
extern "C" {
#endif

/***********************************************/
/* Masking macros for API functions.           */
/* These avoid the function call overhead,     */
/* when putting and getting single characters. */
/***********************************************/

#define ggetc(fp)  (((fp)->count_r > 1) ? \
   (fp)->count_r--, (int)*(unsigned char*)((fp)->buf++) : (ggetc)((fp)))

#define gputc(c, fp)  (((fp)->count_w > 1) ? \
   (fp)->count_w--, *(unsigned char*)((fp)->buf++) = (c) : (gputc)((c), (fp)))

#define gio_init() ((gio.initialized != GIO_MAGIC) ? (gio_init)(): 0)

typedef HOST_INT6_T gpos_t; /* API stream byte position, > 2 gigabyte */

/* GIO stream OPEN */
GFILE * (gopen)(        /* i/o stream (error == NULL) */
   const char *name,    /* path and file name */
   const char *mode);   /* mode string */

/* GIO stream OPEN Temp file */
GFILE * (gopen_tempfile)(/* i/o stream (error == NULL) */
   const char **name,    /* path and file name */
   const char *mode);    /* mode string */

/* GIO stream file Descriptor OPEN */
GFILE * (gdopen)(       /* i/o stream (error == NULL) */
   int fd,              /* file descriptor */
   const char *mode);   /* mode string */

/* GIO stream REOPEN */
GFILE * (greopen)(      /* i/o stream (error == NULL) */
   const char *name,    /* path and file name */
   const char *mode,    /* mode string */
   GFILE *fp);          /* old i/o stream */

/* GIO stream FILE descriptor Number */
int (gfileno)(          /* file descriptor */
   GFILE *fp);          /* file pointer */

/* GIO stream End Of File status */
int (geof)(             /* non-zero = EOF is set */
   GFILE *fp);          /* file pointer */

/* GIO stream SETBUF */
void (gsetbuf)(
   GFILE *fp,           /* i/o stream */
   void *buf);          /* buffer pointer */

/* GIO stream SETVBUF */
int (gsetvbuf)(         /* status (error != 0) */
   GFILE *fp,           /* i/o stream */
   void *buf,           /* buffer pointer (ignored) */
   int mode,            /* _IOFBF, _IOLBF, _IONBF */
   size_t size);        /* buffer size */

/* GIO stream FLUSH */
int (gflush)(           /* status (error != 0) */
   GFILE *fp);          /* i/o stream */

/* GIO stream CLOSE */
int (gclose)(           /* status (error != 0) */
   GFILE *fp);          /* i/o stream, NULL = all streams */

/* GIO stream GET stream Character */
int (ggetc)(            /* next character (error == EOF) */
   GFILE *fp);          /* i/o stream */

/* GIO stream READ Malloc pointer */
int (greadm)(           /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   size_t size);        /* requested byte count */

/* GIO stream READ Aligned pointer */
int (greada)(           /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   size_t size,         /* requested byte count */
   GIO_ALIGN align);    /* buffer alignment */

/* GIO stream READ Biased pointer */
int (greadb)(           /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   size_t size,         /* requested byte count */
   GIO_ALIGN align,     /* buffer alignment */
   size_t bias);        /* alignment bias */

/* GIO stream READ copy buffer */
int (greadc)(           /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   void *buf,           /* read buffer pointer */
   size_t size);        /* requested byte count */

/* GIO stream UNREAD copy buffer */
int (gunread)(          /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void *buf,     /* unread buffer pointer */
   size_t size);        /* requested byte count */

/* GIO stream PUT Character */
int (gputc)(            /* next character (error == EOF) */
   int c,               /* next character to write */
   GFILE *fp);          /* i/o stream */

/* PUT String */
int (gputs)(            /* zero (error == EOF) */
   const char *str,     /* character string to write */
   GFILE *fp);          /* i/o stream */

/* GIO stream WRITE Malloc pointer */
int (gwritem)(          /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   size_t size);        /* requested byte count */

/* GIO stream WRITE Aligned pointer */
int (gwritea)(          /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   size_t size,         /* requested byte count */
   GIO_ALIGN align);    /* buffer alignment */

/* GIO stream WRITE Biased pointer */
int (gwriteb)(          /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   size_t size,         /* requested byte count */
   GIO_ALIGN align,     /* buffer alignment */
   size_t bias);        /* alignment bias */

/* GIO stream WRITE copy buffer */
int (gwritec)(          /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   const void *buf,     /* write buffer pointer */
   size_t size);        /* requested byte count */

/* GIO stream long SEEK offset */
int (gseek)(            /* status (error != 0) */
   GFILE *fp,           /* i/o stream */
   long offset,         /* long seek offset */
   int whence);         /* SEEK_SET, SEEK_CUR, SEEK_END */

/* GIO stream long TELL current offset */
long (gtell)(           /* status (error != 0) */
   GFILE *fp);          /* i/o stream */

/* GIO stream GET current POSition (offset) */
int (ggetpos)(          /* status (error != 0) */
   GFILE *fp,           /* i/o stream */
   gpos_t *offset);     /* huge seek offset */

/* GIO stream REWIND (offset == 0) */
void (grewind)(         /* status (error != 0) */
   GFILE *fp);          /* i/o stream */

/* GIO stream SET POSition (offset) */
int (gsetpos)(          /* status (error != 0) */
   GFILE *fp,           /* i/o stream */
   const gpos_t *offset); /* huge seek offset */

/* GIO stream CLEAR ERRor and eof */
void (gclearerr)(
   GFILE *fp);          /* i/o stream */

/* GIO stream EOF state */
int (geof)(             /* zero or EOF */
   GFILE *fp);          /* i/o stream */

/* GIO stream ERROR state */
int (gerror)(           /* zero, or last non-zero errno */
   GFILE *fp);          /* i/o stream */

/* GIO INITialization */
int (gio_init)(void);

/* GIO open_max manipulation */
int (gopenmax)(         /* set or get gio.open_max (error < 0) */
   GIO_OPEN_MAX_OPTION option,
   ...);

/* GIO fctnl */
int (gfcntl)(           /* manipulate file descriptor */
   GFILE *fp,           /* i/o stream */
   int cmd,             /* command */
   /* int arg */... );  /* command argument */

/* one, and only one, module should #define GIO_GLOBAL */
#ifndef GIO_GLOBAL

   extern int gio_mpi_node;  /* check if opening stdin, stdout or stderr 
                                on mpi node >= 0 */

   extern GFILE * const gstdin;
   extern GFILE * const gstdout;
   extern GFILE * const gstderr;

   extern GIO_ROOT gio;

#endif /* GIO_GLOBAL */

#define _IOCOPY_READ	GIO_IO_COPY_READ
#define _IOCOPY_WRITE	GIO_IO_COPY_WRITE
#define _IOMMAP_READ	GIO_IO_MMAP_READ
#define _IOMMAP_WRITE	GIO_IO_MMAP_WRITE
#define _IORAW_READ	GIO_IO_RAW_READ
#define _IORAW_WRITE	GIO_IO_RAW_WRITE
#define _IONON_TUNED	GIO_IO_NON_TUNED
#define _IONON_CACHE	GIO_IO_NON_CACHE

#ifdef  __cplusplus
}
#endif

#endif /* __CGIO_H */
