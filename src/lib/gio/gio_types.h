#ifndef __GIO_TYPES_H
#define __GIO_TYPES_H

#include <stddef.h>

/*
RCS-t=GIO, "private" portion of Application Program Interface.

$Id: gio_types.h 13 2009-01-06 16:56:43Z user $
*/

typedef ptrdiff_t  GIO_BIG;     /* big signed integer for buffer counts. */

typedef enum
{  /* boolean values, for convenience */
   GIO_FALSE = 0,
   GIO_TRUE = 1,
   GIO_NO = 0,
   GIO_YES = 1
} GIO_BOOLEAN;

typedef enum
{  /* bit flags defining stream state */
   GIO_STATE_OPENED   = 1 << 0,       /* initialization completed */
   GIO_STATE_KEEPFB   = 1 << 1,       /* don't free GFILE upon close */
                                      /*    Set for static structures */
                                      /*    such as gstdin, out, err. */
   GIO_STATE_OPENR    = 1 << 2,       /* opened for reading */
   GIO_STATE_OPENW    = 1 << 3,       /* opened for writing */
   GIO_STATE_OPENA    = 1 << 4,       /* writes append to end */
   GIO_STATE_TRUNCATE = 1 << 5,       /* truncate existing file */
   GIO_STATE_CREATE   = 1 << 6,       /* create new file */
   GIO_STATE_SEEK     = 1 << 7,       /* seek is supported */
   GIO_STATE_BINARY   = 1 << 8,       /* binary stream (not text) */
   GIO_STATE_BLOCK    = 1 << 9,       /* ??? block (not char) device */
   GIO_STATE_LINEBUF  = 1 << 10,      /* a) line buffered (not full) */
   GIO_STATE_FULLBUF  = 1 << 11,      /* b) full buffering (not line) */
                                      /*    Neither implies non-buffered. */
   GIO_STATE_UNREAD   = 1 << 12,      /* unread buffer active */
                                      /*    If buffered, buf and count_r */
                                      /*    reference the unread buffer, */
                                      /*    while buf_u and count_u save */
                                      /*    the primary buffer state. */
                                      /*    If non-buffered, buf_u and */
                                      /*    count_u reference the unread */
                                      /*    buffer (and count_r == 0). */
   GIO_STATE_COPYR    = 1 << 14,      /* a) kernel read buffer copy */
   GIO_STATE_MAPR     = 1 << 15,      /* b) kernel read memory map */
   GIO_STATE_RAWR     = 1 << 16,      /* c) kernel read direct i/o */
                                      /*    initialized by gsetvbuf */
                                      /*    mutually exclusive */
   GIO_STATE_COPYW    = 1 << 17,      /* a) kernel write buffer copy */
   GIO_STATE_MAPW     = 1 << 18,      /* b) kernel write memory map */
   GIO_STATE_RAWW     = 1 << 19,      /* c) kernel write direct i/o */
                                      /*    initialized by gsetvbuf */
                                      /*    mutually exclusive */
   GIO_STATE_READ     = 1 << 20,      /* a) stream is now reading */
   GIO_STATE_WRITE    = 1 << 21,      /* b) stream is now writing */
                                      /*    mutually exclusive */
   GIO_STATE_EOF      = 1 << 22,      /* read end-of-file indicator. */
                                      /*    Visible to GIO API (geof). */
                                      /*    Set when read API requests */
                                      /*       1 or more bytes and API */
                                      /*       returns zero bytes. */
                                      /*    Reset by "clearerr", "seek", */
                                      /*       "setpos", AND a read that */
                                      /*       transfers data (perhaps */
                                      /*       following a tape EOF!). */
   GIO_STATE_KEOF     = 1 << 23,      /* kernel end-of-file detected */
                                      /*    on an input file descriptor. */
                                      /*    Set when the kernel cannot */
                                      /*       provide any more data. */
                                      /*    Reset when the kernel might */
                                      /*       have more data. */
   GIO_STATE_CLOSED   = 1 << 30       /* closed, but formerly open (debug) */
} GIO_STATE;

typedef enum
{  /* Miscellaneous constants, non-preprocessor and debugable. */

   /* Minimum 2**N modulo for cache buffer size (non-block and non-page). */
   GIO_MALLOC_MIN = GIODEF_MALLOC_MIN,

   /* STATE mask for buffered versus non-buffered state */
   GIO_MASK_BUFFERED = (GIO_STATE_LINEBUF | GIO_STATE_FULLBUF),

   /* STATE mask for kernel buffer copy semantics */
   GIO_MASK_COPIED = (GIO_STATE_COPYR | GIO_STATE_COPYW),

   /* STATE mask for kernel memory mapped semantics */
   GIO_MASK_MAPPED = (GIO_STATE_MAPR | GIO_STATE_MAPW),

   /* STATE mask for kernel raw/direct i/o semantics */
   GIO_MASK_RAWIO = (GIO_STATE_RAWR | GIO_STATE_RAWW),

   /* STATE mask for active (read or write) versus idle state */
   GIO_MASK_ACTIVE = (GIO_STATE_READ | GIO_STATE_WRITE),

   /* setvbuf "non-standard" flags for GIO extensions */
   GIO_NOMINAL_BUFFERING     = 1 << 19,      /* nominal buffering */

   /* setvbuf "advisory" flags (may be ignored) for GIO extensions */
   /* Default is the fastest available (copy, memory map or raw) */
   GIO_IO_COPY_READ     = 1 << 20,      /* a) kernel read buffer copy */
   GIO_IO_MMAP_READ     = 1 << 21,      /* b) kernel read memory map */
   GIO_IO_RAW_READ      = 1 << 22,      /* c) kernel read direct i/o */
                                        /*    mutually exclusive */
   GIO_IO_COPY_WRITE    = 1 << 23,      /* a) kernel write buffer copy */
   GIO_IO_MMAP_WRITE    = 1 << 24,      /* b) kernel write memory map */
   GIO_IO_RAW_WRITE     = 1 << 25,      /* c) kernel write direct i/o */
                                        /*    mutually exclusive */

   /* gsetvbuf development flags (not intended for production usage) */
   GIO_IO_NON_TUNED   = 1 << 26,        /* constant, non-zero cache size. */
                                        /*    speed test for given size */
   GIO_IO_NON_CACHE   = 1 << 27,        /* constant, zero cache size. */
                                        /*    overhead for tuning logic, */
                                        /*    relative to non-buffered. */
                                        /*    mutually exclusive */

   /* gsetvbuf "advisory" mask (supported kernel i/o semantics) */
   GIO_MASK_SUPPORTED_MODES = GIODEF_MASK_SUPPORTED_MODES,

   /* gsetvbuf "advisory" flags (optimized for speed) */
   GIO_DEFAULT_SPEED_MODE = GIODEF_DEFAULT_SPEED_MODE,

   /* gsetvbuf mask for standard flags */
   GIO_MASK_STANDARD_MODES    = _IOFBF | _IOLBF | _IONBF,

   GIO_GB = 1024 * 1024 * 1024,       /* bytes per gigabyte */
   GIO_MB = 1024 * 1024,              /* bytes per megabyte */
   GIO_KB = 1024,                     /* bytes per kilobyte */
   GIO_BITS_PER_BYTE = 8              /* bits per byte */
} GIO_MISC;

typedef enum
{  /* media type of i/o stream.  Used to influence defaults. */
   GIO_MEDIA_GENUS_UNKNOWN,      /* ??? */
   GIO_MEDIA_GENUS_TTY,          /* interactive device */
   GIO_MEDIA_GENUS_DISK,         /* disk (regular) file */
   GIO_MEDIA_GENUS_TAPE,         /* tape (char) device */
   GIO_MEDIA_GENUS_BLOCK,        /* ??? (block) device */
   GIO_MEDIA_GENUS_FIFO,         /* fifo (unamed pipe) */
   GIO_MEDIA_GENUS_SOCKET,       /* fifo (named pipe) */
   GIO_MEDIA_GENUS_DIR           /* directory */
} GIO_MEDIA_GENUS;


typedef struct GFILE
{  /* file control structure, referenced by stream */

   /***********************************************************************/
   /*  <------- BUFFER (primary and mmap) -------->                       */
   /*                                                                     */
   /*                                                                     */
   /*                          logical offset                             */
   /*                          ^                                          */
   /*                          ^                                          */
   /*                          buf{_u}     buf+count_{r|w|u}              */
   /*                          ^           ^                              */
   /*                          ^           ^                              */
   /*  AAaaAAaaAAaaXXxxXXxxXXxxOOooOOooOOooPPppPPpp                       */
   /*  ^           ^                       ^       ^                      */
   /*  ^           ^                       ^       ^                      */
   /*  base_{p|m}  start                   ^       base_{p|m}+size_{p|m}  */
   /*              ^                       ^                              */
   /*              ^                       ^                              */
   /*              offset                  ^                              */
   /*              ^                       ^                              */
   /*              ^                       ^                              */
   /*              write offset            read offset                    */
   /*                                                                     */
   /*  AAaa = alignment (malloc'ed, but not used)                         */
   /*  XXxx = processed (already read or written by API)                  */
   /*  OOoo = pending (waiting read or write by API)                      */
   /*  PPpp = pad (malloc'ed, but not used)                               */
   /*                                                                     */
   /***********************************************************************/

   /* structure member sequence choosen to improve cache hits */
   unsigned magic;      /* magic number for GIO */

   /* primary (or unread) buffer, logical i/o description */
   GIO_BIG count_r;     /* byte count remaining in read (or unread) buffer */
   GIO_BIG count_w;     /* byte count remaining in write buffer. */
   char *start;         /* pointer to start of data in base_{p|m}. */
   char *buf;           /* pointer to next logical byte in base_{p|m|u}. */

     /* When writing, GIO_STATE_WRITE is set and count_r == 0.
        count_w == vacant space remaining in the write buffer.
        start is the first byte waiting to be flushed.
        buf is the end of the data waiting to be flushed, and
        the first vacant byte waiting to be used by the API.
        Data is waiting to be flushed, if buf > start.

        When reading, GIO_STATE_READ is set and count_w == 0.
        count_r == valid data remaining in read (or unread) buffer.
        Buf is the next byte waiting to be consumed by the API.
        It may point into either the primary or unread buffer.
        Start is the first byte of valid data in primary buffer.

        When reading, unread data exists, if GIO_STATE_UNREAD is set.
        buf points into the unread buffer, and count_r refers
        to the unread data waiting to be consumed by the API.
        buf_u and count_u remember the buf and count_r values
        in the primary buffer, until the unread data is consumed.

           An exception to these rules is made for non-buffered access.
           In this case, count_r and count_w are always zero (nothing
           is cached).   When reading, if GIO_STATE_UNREAD is set, then
           buf_u and count_u refer to the unread buffer (not primary).
           The entire unread cache is consumed by the first read with
           a non-zero size, i.e. extra unread data is discarded.
     */

   GIO_STATE state;           /* flags */
   int fd;                    /* file descriptor (handle) */

   /* API function pointers, with stream specific optimization */ 
   int (*greadm)(struct GFILE *fp, const void **buf, GIO_BIG size);
   int (*greada)();  /* deferred */
   int (*greadb)(struct GFILE *fp, const void **buf, GIO_BIG size,
            GIO_ALIGN align, size_t bias);
   int (*greadc)(struct GFILE *fp, void *buf, GIO_BIG size);
   int (*gunread)(struct GFILE *fp, const void *buf, size_t size);
   int (*gflushr)(struct GFILE *fp);
   int (*gwritem)(struct GFILE *fp, void **buf, GIO_BIG size);
   int (*gwritea)();  /* deferred */
   int (*gwriteb)(struct GFILE *fp, void **buf, GIO_BIG size,
            GIO_ALIGN align, size_t bias);
   int (*gwritec)(struct GFILE *fp, const void *buf, GIO_BIG size);
   int (*gflushw)(struct GFILE *fp);

   /* primary buffer, allocated memory description */
   char *base_p;        /* malloc pointer to primary buffer */
   GIO_BIG size_p;      /* total byte size of primary buffer (base_p) */
                        /* base_p may be used for reading, or */
                        /* writing, or both, or neither. */
                        /* Once allocated (perhaps for write mode), */
                        /* memory is retained even while mmap is */
                        /* used (perhaps for read mode). */
   char *base_m;        /* mmap pointer to primary buffer */
   GIO_BIG size_m;      /* total byte size of mmap buffer (base_m) */
                        /* base_m may be used for reading, or */
                        /* writing, or both, or neither. */
                        /* Memory is mapped, only while being used. */
                        /* Otherwise base_m == NULL, and size_m == 0. */
   HOST_INT6_T offset;  /* stream byte offset to GFILE.start */
                        /* The API stream offset is equal to... */
                        /*    logical = offset + (buf - start) */
                        /*       unless the unread buffer contains data. */
                        /* When reading, the Unix stream offset is... */
                        /*    reading = offset + (buf - start) + count_r */
                        /* When writing, the Unix stream offset is... */
                        /*    writing = offset */
   HOST_INT6_T eof_size; /* byte offset to stream end-of-file */

   /* secondary buffer, logical write description */
   /* The secondary buffer is used for writing only if the nominal */
   /* alignment in the primary buffer is not satisfactory. */
   GIO_BIG count_s;     /* byte count of pending write, non-zero */
                        /*    implies buf_s and buf_w are non-NULL */
   char *buf_s;         /* pointer to write buffer (aligned) */
                        /*    non-NULL implies count_s non-zero */
                        /*    and buf >= start + count_s (non-empty) */
   char *buf_w;         /* pointer to primary buffer (unaligned) */
                        /*    memcpy(buf_w, buf_s, count_s) */

   /* secondary buffer, allocated memory description */
   /* The secondary buffer is used for reading or writing, if the nominal */
   /* alignment in the primary buffer is not satisfactory. */
   /* If an API read request spans the primary and unread buffer, */
   /* then the secondary buffer may be used to assemble the two pieces. */
   char *base_s;        /* malloc pointer to secondary buffer */
   GIO_BIG size_s;      /* total byte size of secondary buffer */

   /* kernel i/o block size, nominal values (dynamically tuned) */
   GIO_BIG cache_r;     /* nominal read  size in bytes */
   GIO_BIG cache_w;     /* nominal write size in bytes */
                        /*    mmap and rawio typically require */
                        /*       0 == (io_size % mod_{r|w}) */
   GIO_BIG waste;       /* potential waste from caching (in bytes) */
                        /*    used to dynamically tune cache_{r|w}. */
                        /*    if "n_flush" is significantly greater than */
                        /*    (less than) waste, then "bigger is better" */
                        /*    ("smaller is smarter"), and cache_{r|w} */
                        /*    is increased (decreased). */
                        /*    waste may become negative, if random */
                        /*    access reads reuses existing cache. */
                        /*    waste is reset when READ or WRITE is reset. */
                        /*    if waste is zero, tuning is skipped. */
   int delta;           /* last change in cache size (modulo units) */
                        /*    if positive, the size was increased. */
                        /*    if negative, the size was decreased. */
                        /*    if zero, cache size stayed the same. */
                        /*    subsequent changes in the same direction */
                        /*    accelerate (double), until min or max. */
                        /*    reversals decelerate (minus halve delta). */
                        /*    changes from zero, start with +1 or -1. */

   /* NOTE: big_r, max_r, big_w, max_w should be "tuned" for each host */
   /* and possible for unique files systems on a host. */
   /* This information can be provided by "tuneup" using "test = 3;". */
   /* The results must be incorporated into gsetvbuf.c logic. */

   /* i/o block/page size and tuning limits (set by gsetvbuf) */
   GIO_BIG align_p;     /* base_p buffer alignment 2**N constraint */
                        /*    typical = max(mod_r, mod_w, pagesize) */
   GIO_BIG mod_p;       /* base_p buffer size 2**N modulo constraint */
                        /*    typical = max(mod_r, mod_w) */

                        /* constraints,  1 <= mod <= min <= big <= max */

   GIO_BIG mod_r;       /* cache_r read size 2**N modulo constraint */
                        /*    typical = {blksize | pagesize | minsize} */
   GIO_BIG min_r;       /* lower bound for cache_r read size */
                        /*    zero is optimum for random access. */
                        /*    non-zero implies kernel constraints */
   GIO_BIG max_r;       /* upper bound for cache_r read size */
                        /*    marginal improvement if bigger */
   GIO_BIG big_r;       /* lower bound for read cache by-pass */
                        /*    typical = K * {blksize | pagesize | minsize} */

   GIO_BIG mod_w;       /* cache_w write size 2**N modulo constraint */
                        /*    typical = blksize, pagesize, minsize */
   GIO_BIG min_w;       /* lower bound for cache_w write size */
                        /*    zero is optimum for random access. */
                        /*    non-zero implies kernel constraints */
   GIO_BIG max_w;       /* upper bound for cache_w write size */
                        /*    marginal improvement if bigger */
   GIO_BIG big_w;       /* lower bound for write cache by-pass */
                        /*    typical = K * {blksize | pagesize | minsize} */

   /* general i/o constraints (set by gsetvbuf) */
   GIO_BIG blksize;     /* optimum block size, reported by fstat */

   /* raw (direct) i/o constraints (set by gsetvbuf) */
   size_t blkalign;     /* block memory alignment */
   GIO_BIG minsize;     /* minimum block size and seek 2**N modulo */
   GIO_BIG maxsize;     /* maximum block size */

   int error;           /* errno indicator, last non-zero code */

   /* If buffered, this saves buf and count_r for the primary buffer.
      If non-buffered, this describes the unread buffer description.
      count_u and buf_u are valid only while GIO_STATE_UNREAD is set. */
   GIO_BIG count_u;     /* saved count_r, or unread buffer count. */
   char *buf_u;         /* saved buf pointer, or unread buffer pointer. */

   /* unread buffer, allocated memory description */
   char *base_u;        /* malloc pointer to unread buffer */
   GIO_BIG size_u;      /* total byte size of unread buffer */

   struct GFILE *chain; /* single linked list of open files */
   GIO_MEDIA_GENUS genus; /* apparent media (disk, tape, pipe, ...) */

   struct
   {  /* cached file descriptor parameters */
      int open_flags, open_mode;
      char name[GIO_FILENAME_MAX+1];
      GIO_BOOLEAN really_open;   /* current fd is valid */
      GIO_BOOLEAN tmp_close_ok;  /* okay to close temporarily */
      GIO_BOOLEAN initialized;   /* GIO_gopen_init has been done */
      unsigned long last_access_count;  /* last access counter */
      HOST_INT6_T last_open_pos; /* position when last open */
      int last_close_status;	 /* status when last closed */
   } cfd;

} GFILE;

typedef struct GIO_ROOT
{  /* Geoscience/Gorgeous/Grotesque IO package, root descriptor */
   unsigned initialized;        /* initialized with magic */
   GIO_BIG page_size;           /* memory page size (non-zero !) */
   FILE *std_debug;             /* debug print file pointer */
   FILE *trc_file;              /* I/O Trace file pointer (if used) */
   GFILE *open_chain;           /* open chain */
   GFILE *closed_chain;         /* closed chain (debug support) */
   int num_fd_open;	           /* number of really open CFDs */
   int open_max;                /* soft limit on really open CFDs */
   int init_open_max;           /* initial value of open_max */
   HOST_ULLONG_T access_age;    /* cache file descriptor, serial number */
   long clk_tck;                /* run-time conversion of ticks to seconds */
   long blk_size;               /* fixed I/O buffer size: set from env GIO_BLKSIZE */
   int verbose;                 /* verbose: set from env GIO_VERBOSE */
} GIO_ROOT;

#endif /* __GIO_TYPES_H */
