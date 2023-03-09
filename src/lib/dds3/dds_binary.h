#ifndef __DDS_BINARY_H
#define __DDS_BINARY_H
 
/*
RCS-t=DDS, binary data i/o
 
$Id: dds_binary.h 219 2011-02-16 01:11:40Z user $
*/
 
/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/*******************************/
/* macros to map table entries */
/*******************************/
 
#define MAP_PTR(OUT_BIN, IN_BIN)  \
   (dds_map[(OUT_BIN) * dds_bin_max + (IN_BIN)])

/* asp format name */
#define AMOCO_FORMAT  "asp"

/* default format when creating binary data */
#define DEFAULT_FORMAT  AMOCO_FORMAT

/*********************************/
/* typedef and enumerated values */
/*********************************/

typedef enum DDS_USP_FLAGS
{  /* generic trace flags */
   DDS_USP_dummy       = 1 <<  0  /* dummy place holder */
} DDS_USP_FLAGS;
 
typedef enum DDS_SEGY_FLAGS
{  /* generic trace flags */
   DDS_SEGY_charisma   = 1 <<  0, /* Schlumberger SEGY, embedded hdrs */
   DDS_SEGY_NumSmp     = 1 <<  1, /* number of samples */
   DDS_SEGY_SmpInt     = 1 <<  2, /* sample interval */
   DDS_SEGY_SU         = 1 <<  3  /* Delft's SU format */
} DDS_SEGY_FLAGS;
 
typedef struct DDS_SEAM
{  /* expression sequence for read, write, or map evaluation */
   DDS_EXPR *thread;     /* first expression to evaluate */
   DDS_EXPR *needle;     /* last expression (extensible end) */
} DDS_SEAM;

typedef struct DDS_OBJECT_REF
{  /* symbol table object reference */
   DDS_OBJECT *object;      /* trace, line header, card header, ... */
   DDS_TYPE   *type;        /* type pointer for special objects. */
   DDS_OBJECT *r_object;    /* read  object for evaluation */
   DDS_OBJECT *w_object;    /* write object for evaluation */
   DDS_SEAM r_seam;         /* evaluated after  read */
   DDS_SEAM w_seam;         /* evaluated before write */
} DDS_OBJECT_REF;

typedef struct DDS_MEMBER_REF
{  /* symbol table member reference */
   DDS_MEMBER *member;      /* RecNum, TrcNum, TrcType, ... */
   DDS_TYPE   *type;        /* type pointer for special members. */
#if defined(_WIN32) || defined(MINGW)
   size_t       offset;       /* total offset to this member */
#else
   long       offset;       /* total offset to this member */
#endif
} DDS_MEMBER_REF;


typedef struct DDS_BIN_USP
{  /* binary descriptor, usp standard */
   DDS_USP_FLAGS flags;
 
   DDS_OBJECT_REF line;
   DDS_MEMBER_REF line_prefix;
   DDS_MEMBER_REF line_HlhEnt;
   DDS_MEMBER_REF line_HlhByt;
   DDS_MEMBER_REF line_HlhBuf;
   char *line_buf;
   int line_green_min;
   int line_green_max;
} DDS_BIN_USP;
 
typedef struct DDS_BIN_SEGY
{  /* binary descriptor, segy standard */
   DDS_SEGY_FLAGS flags;
 
   DDS_OBJECT_REF card;
   DDS_MEMBER_REF card_prefix;
   DDS_MEMBER_REF card_suffix;
   char *card_buf;
   int card_green_min;
   int card_green_max;
 
   DDS_OBJECT_REF binary;
   DDS_MEMBER_REF binary_prefix;
   DDS_MEMBER_REF binary_suffix;
   DDS_MEMBER_REF binary_NumSmp;
   char *binary_buf;
   int binary_green_min;
   int binary_green_max;
   int sample_prec;

   DDS_MEMBER_REF TrcNumSmp;
   DDS_MEMBER_REF TrcSmpInt;
} DDS_BIN_SEGY;
 
typedef struct DDS_BIN_GEN
{  /* binary descriptor, generic standard: zip-compressed table */
   /* zip-compressed trace location table which gets appended to 
      the end of the binary. */
   int level;            /* Compression level */
#if defined(_WIN32) || defined(MINGW)
   HOST_ULLONG_T size;    /* Current table size */
   HOST_ULLONG_T nmaxtrc; /* Max # traces in table */
   HOST_ULLONG_T ntraces; /* Running # traces in binary */
#else
   HOST_ULONG_T size;    /* Current table size */
   HOST_ULONG_T nmaxtrc; /* Max # traces in table */
   HOST_ULONG_T ntraces; /* Running # traces in binary */  
#endif
   HOST_ULLONG_T offset; /* Running offset pointer in binary */
   HOST_ULLONG_T *table; /* Table of trace offsets */

} DDS_BIN_GEN;

typedef enum DDS_AXIS_FLAGS
{  /* axis control flags */
   DDS_AXIS_SPARSE  = 1 <<  0, /* sparse axis (minimum storage). */
         /* If set, some traces within this axis may be missing, */
         /* i.e. storage may not be reserved for dead traces. */
         /* If size != 1, the "stride" for this axis and */
         /* higher axes can not be precomputed. */
         /* If reset, the axis is DENSE. */
         /* Storage is reserved for all traces. */
         /* Read and write automatically insert dead traces. */
 
   DDS_AXIS_ASYNC   = 1 <<  1, /* asynchronous "index". */
         /* This flag controls synchronization of this sort index */
         /* with changes in the sort index for the next higher axis. */
         /* If set, this index is NOT coupled to the higher axis. */
         /* If reset, the expected value for this index is reset */
         /* whenever the index for the next higher axis changes. */
         /* For example, the expected trace number is reset to one, */
         /* whenever the record index changes. */
 
   DDS_AXIS_SLICE   = 1 <<  2, /* multiplexed across files. */
         /* If set, axis is multiplexed across "size" files. */
 
   DDS_AXIS_UPPER   = 1 <<  3, /* retain traces above  window. */
   DDS_AXIS_WINDOW  = 1 <<  4, /* retain traces within window. */
   DDS_AXIS_LOWER   = 1 <<  5, /* retain traces below  window. */
         /* These flags may be used to select a window of traces. */
         /* Windowing is applied prior to other trace editing. */
         /* An open error is issued, if no ranges are retained. */
 
   DDS_AXIS_ERROR   = 1 <<  6, /* error   if trace "index" not nominal */
         /* Read and write processing terminate, if an error is printed. */
 
   DDS_AXIS_WARN    = 1 <<  7, /* warning if trace "index" not nominal */
         /* Read and write processing continues, if a warning is printed. */
 
   DDS_AXIS_FORCE   = 1 <<  8, /* force trace "index" to expected value. */
         /* If set, the sort index is forced to the expected value. */
 
   DDS_AXIS_REJECT  = 1 <<  9, /* skip trace if "index" not nominal */
         /* If set and the sort index is NOT the expected value, the */
         /* offending trace is dropped and processing continues. */

   DDS_AXIS_PAD     = 1 << 10, /* pad dead traces, when reading sparse. */
         /* If set and the sort index is NOT the expected value, */
         /* dead traces are inserted, even when reading a sparse axis. */

   DDS_AXIS_global  = (DDS_AXIS_SPARSE | DDS_AXIS_ASYNC | DDS_AXIS_SLICE),

   DDS_AXIS_local = (DDS_AXIS_UPPER | DDS_AXIS_WINDOW | DDS_AXIS_LOWER |
      DDS_AXIS_ERROR | DDS_AXIS_WARN | DDS_AXIS_FORCE |
      DDS_AXIS_REJECT | DDS_AXIS_PAD),

   DDS_AXIS_explicit = (DDS_AXIS_global | DDS_AXIS_local),
   /******************************************************************/
   /* End of axis flags that may be set explicitly.                  */
   /*                                                                */
   /* The following flags are used by ddsCubeScan and ddsCubePrint.  */
   /* If set, the corresponding axis parameter is "defined".         */
   /* The flag values overload some bits that are not used until     */
   /* ddsCubeLinker is executed (after Scan and Print are done).     */
   /******************************************************************/

   DDS_AXIS_flag       = 1 << 30,
   DDS_AXIS_size       = 1 << 29,
   DDS_AXIS_origin     = 1 << 28,
   DDS_AXIS_delta      = 1 << 27,
   DDS_AXIS_base       = 1 << 26,
   DDS_AXIS_step       = 1 << 25,

   DDS_AXIS_open = (DDS_AXIS_flag |
      DDS_AXIS_UPPER | DDS_AXIS_WINDOW | DDS_AXIS_LOWER),

   DDS_FLAG_O = DDS_AXIS_open,

   DDS_FLAG_OSA = (DDS_AXIS_open |
      DDS_AXIS_SPARSE | DDS_AXIS_ASYNC |
      DDS_AXIS_UPPER  | DDS_AXIS_WINDOW | DDS_AXIS_LOWER),

   DDS_AXIS_cube = (DDS_AXIS_flag | DDS_AXIS_size |
      DDS_AXIS_origin | DDS_AXIS_delta | DDS_AXIS_base |
      DDS_AXIS_step),
 
   /******************************************************************/
   /* End of axis flags for ddsCubeScan and ddsCubePrint exchange.   */
   /* The following flags are set implicitly from explicit flags.    */
   /******************************************************************/
 
   DDS_AXIS_SORT_SET= 1 << 22, /* Sort value mapping is set.         */
         /* Set by ddsMapAssign, iff this sort field is mapped.      */
         /* Tested by ddsLinkObjectSU2, when binary is frozen.       */
         /* If set, then the mapped value is retained.               */
         /* If reset, then write map defaults to axis ordinal + 1.   */
 
   DDS_AXIS_CHANGE  = 1 << 23, /* enable SYNC check in ddsEditTrace. */
         /* Set and reset by ddsCubeInc as ordinals changes.         */
         /* Reset by ddsEditTrace, if expected ordinal is forced.    */
 
   DDS_AXIS_CYCLE   = 1 << 24, /* synchronous cycle for higher axis. */
         /* Set by ddsEditTrace in lower synchronous axes, when the  */
         /* sort index of a higher axis changes.                     */
         /* Reset by ddsEditTrace when the expected API ordinal      */
         /* cycles back to zero again (achieves synchronization).    */
         /* While set, the expected and actual API index differ,     */
         /* causing dead traces to be padded until synchronized.     */

   DDS_AXIS_SORT    = 1 << 26, /* this sort field exists in struct. */
         /* Set, iff a sort field is specified for this axis and the */
         /* default "trace" type contains a member with this name. */
         /* Sort fields are defined by "sort.x= TrcNum", etc. */
 
   DDS_AXIS_SOLID   = 1 << 28, /* compact i/o stream ("stride" is valid). */
         /* Set, iff all lower i/o axes are DENSE or have size == 1. */
         /* If set, precompute stride between elements on this axis. */
         /* Stride is used to explicitly compute seek byte offsets. */
 
   DDS_AXIS_SYNC    = 1 << 29, /* synchronize this axis with next higher. */
         /* Set, iff ASYNC is reset and next higher axis has SORT set. */
         /* If set, the expected sort index for this axis is reset, */
         /* whenever the sort index for the next higher axis changes. */
         /* For example, the expected value for TrcNum is reset to one, */
         /* whenever the value of RecNum (next axis) changes. */
 
   /******************************************************************/
   /* End of axis flags that are established during open processing. */
   /* The following flags are manipulated while processing "traces". */
   /******************************************************************/

   DDS_AXIS_FIXED   = 1 << 30  /* set if trace "index" was FORCEd. */
         /* Set, iff a sort index is forced to the expected value. */

} DDS_AXIS_FLAGS;
 
typedef double DDS_COORD;   /* physical coordinate ("%lg") */

typedef struct DDS_AXIS
{  /* axis description for hyper-cubes */
   DDS_AXIS_FLAGS flag;     /* flags associated with this axis. */
 
   const char *name;        /* axis name. */
                            /*    required for each axis. */
   long size;               /* maximum number of points. */
                            /*    greater than zero required. */
   DDS_COORD origin;        /* physical start */
                            /*    default is zero. */
   DDS_COORD delta;         /* physical increment */
                            /*    default is one, non-zero required. */
   const char *units;       /* physical units */
                            /*    default is zero length string. */
   DDS_COORD base;          /* starting "index" coordinate */
                            /*    default is zero. */
   DDS_COORD step;          /* "index" coordinate increment */
                            /*    default is one, non-zero required */
   const char *sort_name;   /* sort field name. */
                            /*    default is zero length string. */
   long ngrid; double *grid;/* irregular grid definition. */
                            /*    default is zero points. */
 
   /************************************************************/
   /* End of explicit attributes provided by dictionary.       */
   /* Beginning of implicit attributes set by open processing. */
   /************************************************************/
 
   long ordinal;            /* expected ordinal of sort "index". */
                            /*    0 <= ordinal < size */

   DDS_MEMBER_REF sort;     /* sort field decription */

   HOST_LLONG_T stride;     /* linear offset between elements. */
                            /*    If DDS_AXIS_SLICE is set, then value */
                            /*    is a slice count for bin->slice[]. */
                            /*    If reset, value is a byte count */
                            /*    for random access into slice. */
   HOST_INT6_T stripe;      /* linear offset between elements. */
                            /*    Byte stride across sliced files. */
   HOST_LLONG_T seek_min;   /* seek minimum stride (search scheme) */
   HOST_LLONG_T seek_max;   /* seek maximum stride (search scheme) */
                            /*    Deferred...  if an axis is sparse, */
                            /*    then seek could examine the sort */
                            /*    value to find an existing trace. */
                            /*    This process might use a binary search */
                            /*    and estimated min/max seek values. */
} DDS_AXIS;
 
typedef struct DDS_CUBE
{  /* hyper-cube description */
   int rank;        /* number of hyper cube dimensions */
   int io_rank;     /* first rank in the i/o stream. */
                    /*    Mimimum value is 1. */
                    /*    Nominal value is 2. */
   double chunk;    /* nominal media length for output margin. */
                    /*    Required for EOM early warning. */
   int cleave;      /* nominal rank for output media change */
                    /*    io_rank <= cleave <= rank. */
                    /*    Nominal value is 2. */
   DDS_AXIS *axis;  /* zero plus 1 through rank */
                    /*    cube.axis[0] is used for special housekeeping, */
                    /*    i.e. it does NOT describe a hyper cube axis. */
} DDS_CUBE;

 
typedef enum DDS_BIN_STATE
{  /* Bit flags defining binary data state */
   DDS_BIN_OPEN_CREATE = 1 <<  0,  /* Create new data and format. */
                                   /*    If reset, access existing data. */
                                   /*    Implied by open's "mode". */
   DDS_BIN_FREEZE      = 1 <<  1,  /* Freeze dictionary contents. */
                                   /*    Set during open processing. */
                                   /*    Reset the first time binary data */
                                   /*       is accessed (read/write/seek). */
   DDS_BIN_CRAM        = 1 <<  2,  /* Line header on first media only. */
                                   /*    If set, only the first tape */
                                   /*    begins with a line header and */
                                   /*    the bytes within a single trace */
                                   /*    may span two tapes (usp style). */
   DDS_BIN_UNBLOCK     = 1 <<  3,  /* Unblocked physical i/o on media. */
                                   /*    Implies variable length records. */
   DDS_BIN_FILES       = 1 <<  4,  /* Merge/split files within one media. */
                                   /*    Implies embedded eof and headers. */
   DDS_BIN_TAPE        = 1 <<  5,  /* Tape protocal, mounted media. */
   DDS_BIN_TMP         = 1 <<  6,  /* Temporary file. */
   DDS_BIN_FORCE_FMT   = 1 <<  8,  /* Explicit override for the format. */
                                   /*    Override slice format, iff set. */

   DDS_green_prefix    = 1 << 10,  /* logical Record Length prefix */
   DDS_green_suffix    = 1 << 11,  /*    "       "      "   suffix */
                                   /*    If set, read functions validate */
                                   /*    the associated green word. */

   DDS_zero_dead       = 1 << 12,  /* Zero "Samples" if trace is dead. */
                                   /*    If DDS_zero_dead is set */
                                   /*    and if DDS_drop_dead is reset */
                                   /*    then the Samples are zeroed */
                                   /*    if a dead trace is read or written. */

   DDS_drop_dead       = 1 << 13,  /* drop traces flagged as dead */
   DDS_drop_live       = 1 << 14,  /*   "     "       "    " live */
   DDS_drop_aux        = 1 << 15,  /*   "     "       "    " aux */
                                   /*    Dropped before any trace editting. */

   DDS_TRACE_READ      = 1 << 16,  /* trace_buf contains an input  trace. */
   DDS_TRACE_WRITE     = 1 << 17,  /* trace_buf contains an output trace. */
                                   /*    Reset when trace_buf is flushed. */
                                   /*    Read and write mutually exclusive. */

   DDS_BIN_DESEEK      = 1 << 18,  /* Delayed seek on sibling slice. */
                                   /*    Set by ddsSeekType in each slice */
                                   /*    when seeking on the master binary. */
                                   /*    Reset by ddsSeekType on a slice, */
                                   /*    when it is actually updated. */
                                   /*    Checked before read and write to */
                                   /*    synchronize position as needed. */

   DDS_BIN_EOF         = 1 << 19,  /* end-of-file  state */
                                   /*    Set if open "mode" creates data, */
                                   /*       or by a zero length read, */
                                   /*       or by writing a file mark, */
                                   /*       or by loading media (tape). */
                                   /*    Reset by successful read, write, */
                                   /*       or seek. */
                                   /*    If set, when accessing a trace */
                                   /*    (read, write, or seek), then the */
                                   /*    line header (if any) is processed */
                                   /*    automatically before the trace. */
   DDS_BIN_EOM         = 1 << 20,  /* end-of-media state */
                                   /*    Set by two sequential EOF marks. */
                                   /*    Reset by successful read, write, */
                                   /*       seek, or media (tape) load. */

   DDS_BIN_PADDING     = 1 << 21,  /* Inhibit trace read and write, while */
                                   /*    padding the final traces. */
                                   /*    Set when closing the binary, if */
                                   /*       dead traces must be appended */
                                   /*       to complete a hyper-cube. */
                                   /*    Set by read when end-of-media is */
                                   /*       detected on the final media. */
                                   /*    Reset by seek. */

   DDS_BIN_SOUGHT      = 1 << 22,  /* Synchronize expected sort index. */
                                   /*    Set by successful ddsSeekType. */
                                   /*    Reset by ddsEditTrace, after */
                                   /*       after forcing the expected */
                                   /*       ordinal to the buffer value */
                                   /*       beyond the SOLID cube axes. */

   DDS_BIN_WRITTEN     = 1 << 23,  /* Check padding, if binary is closed. */
                                   /*    Set by successful write trace. */
                                   /*    Reset by trace read or seek. */

   DDS_BIN_MSG_READ    = 1 << 24,  /* read  message. */
                                   /*    set by cdds_read. */
                                   /*    reset by ddsBinMessage. */
   DDS_BIN_MSG_WRITE   = 1 << 25,  /* write message. */
                                   /*    set by cdds_write. */
                                   /*    reset by ddsBinMessage. */
   DDS_BIN_MSG_SEEK    = 1 << 26,  /* seek message. */
                                   /*    set by cdds_seek8. */
                                   /*    reset by ddsBinMessage. */

   DDS_BIN_MSG = (DDS_BIN_MSG_READ | DDS_BIN_MSG_WRITE),

   DDS_BIN_ZIP         = 1 << 27,  /* Binary iS compressed by ddsZIP (modified gzip) */
                                   /*    (only used by generic format) */

   DDS_BIN_RESERVED    = 1 << 28,  /* Not opened, but reserved to be open */
                                   /*    Set by ddsReserveBin and reset by */
                                   /*    ddsDelayedOpen.  Reserves a BIN */
                                   /*    for the Convenience Routines. */

   DDS_EDIT_SEEK       = 1 << 29,  /* edit traces in stream after a seek. */
                                   /*    set by open, setpos and seek. */
                                   /*    reset by read and readedit. */
                                   /*    Implies the initial trace will */
                                   /*    provide valid "index" coordinates. */

   DDS_BIN_CLOSED      = 1 << 30   /* Closed, but formerly open */
                                   /*    Reset by OpenBin, set by CloseBin. */
                                   /*    Inhibit write of flush trace */
                                   /*    while appending to a cube. */

} DDS_BIN_STATE;


typedef struct DDS_BIN
{  /* binary descriptor (one per open binary) */
   const char *name;            /* data filename/mnemonic. */
                                /*    multiple names, iff data is sliced. */
   const char *alias_name;      /* logical name, associate with dict. */
                                /*    "in" assuming "in= dict_name". */
   BIN_TAG tag;                 /* logical bin tag */
   struct DDS_BIN *master;      /* master binary descriptor. */
                                /*    non-NULL iff slave of master. */
   struct DDS_MEDIA *media;     /* current media descriptor. */
   int block_size;              /* physical i/o block size in bytes. */
                                /*    == 0, if unblocked. */
                                /*    == -1, if nominal for media. */
   const char *mode;            /* "r", "r+", ... */
                                /*    as specified by open parameter. */
   DDS_BIN_STATE state;         /* binary flags and state */

   DDS_MEDIA_FMT fmt;           /* enumerated summary of formats. */
                                /*    DDS_FMT_GENERIC, _USP, _SEGY, etc. */
   const char *fmt_name;        /* name of format (first keyword) */
                                /*    "asp", "usp", "segy", etc. */
   const char *old_format;      /* old format name (default for new). */

   const char *dict_open;       /* dict list, processing history. */
   const char *dict_fmt;        /* dict list, format recipes. */
   const char *dict_map;        /* dict list, header map defaults. */
 
   BIN_TAG bin_tag;             /* bin tag: used for convenience routines */
   const char *value;           /* value: used for convenience routines */
   const char *title;           /* title: used for convenience routines */
 
   DDS_DICT *data_dict;         /* dict, iff binary data is attached. */
                                /*    NULL if they use seperate media. */

   HOST_INT6_T slice_tic;       /* number of bytes within one slice. */
                                /*    Value = N, if first io axis is sliced. */
                                /*       Where "N" = bytes per trace. */
                                /*    Value = N * size(x) * size(y), */
                                /*       given axis= t x y A z B */
                                /*       Where "A" is the first sliced axis. */
   HOST_INT6_T slice_toc;       /* bytes remaining within current slice. */
                                /*    When slice_toc decrements to zero, */
                                /*    then slice_now is incremented, i.e. */
                                /*    traces in the next file are accessed. */
   int slice_max;               /* number of slices (files) per binary. */
                                /*    Value is one, if not sliced. */
                                /*    Value = size(A) * size(B), */
                                /*       given axis= t x y A z B */
                                /*       where "A" and "B" are sliced. */
   int slice_now;               /* current slice within binary. */
                                /*    Always less than slice_max. */
                                /*    Value is subscript into slice table. */
   struct DDS_BIN **slice;      /* array of slices (binary slaves). */
                                /*    Non-NULL, iff master of slaves. */
                                /*    Dimensions are [size(B)][size(A)]. */

   DDS_TABLE symbol;            /* compiler symbol table */
   DDS_STACK relocate;          /* array of (DDS_VALUE*) where each */
                                /*    value has a reloc stack that */
                                /*    must be cleaned up by ddsBinClose. */
 
   DDS_BLOCK *block_old;        /* Implicit symbols for old format. */
   DDS_BLOCK *block_new;        /* Explicit symbols for new format. */
 
   DDS_STACK sym_tag;           /* array of (DDS_SYM_TAG*) values. */
                                /*    Subscripts are SYM_TAG values */
                                /*    for symbols exported to API. */
 
   DDS_CUBE cube;               /* hyper-cube description (active). */
                                /*    rank, axis names, size, etc. */
 
   DDS_CUBE *def_cube;          /* hyper-cube description (default). */
                                /*    rank, axis names, size, etc. */
 
   HOST_LLONG_T NextTrc;        /* Next "trace" number within composite. */
                                /*    Increments across slices in data. */
                                /*    Differs from LineTrc iff sliced. */
   HOST_LLONG_T LineTrc;        /* Next "trace" number within slice. */
                                /*    Increments across tapes within slice. */
   HOST_LLONG_T ReelTrc;        /* Next "trace" number within media. */
                                /*    Resets with each new tape. */
   HOST_LLONG_T FileTrc;        /* Next "trace" number within file. */
                                /*    Resets with each new file on tape. */
                                /*    Set by open and seek. */
                                /*    Incremented by read and write. */
                                /*    "Next" values start at 1 and reflect */
                                /*    the media, not the API's view of the */
                                /*    binary data. */

   /* Trace message summary.  Used to avoid an explicit message for each */
   /* stink'n little problem that may be noted while processing data. */
   /* Counts are accumulated and reported at special events, like tape */
   /* mounts, transitions between read and write mode, and upon close. */
   HOST_ULLONG_T MsgTrc;        /* starting offset for summary messages. */
                                /*    none accumulated if negative. */
   int count_gross;             /* accumulated gross  summary */
   int count_warn;              /* accumulated warn   summary */
   int count_force;             /* accumulated force  summary */
   int count_reject;            /* accumulated reject summary */
   int count_dead;              /* accumulated dead   summary */
   int count_fail;              /* accumulated fail   summary */
 
   DDS_OBJECT_REF trace;        /* object and type for default "trace" */
   DDS_MEMBER_REF prefix;       /* green word prefix member */
   DDS_MEMBER_REF suffix;       /* green word suffix member */
   DDS_MEMBER_REF RecNum;       /* "record" number member */
   DDS_MEMBER_REF TrcNum;       /* "trace"  number member */
   DDS_MEMBER_REF TrcType;      /* "trace type" member */
   DDS_MEMBER_REF Samples;      /* "sample" member */
   char *trace_buf;             /* working buffer for one trace */
   char *dead_trace;            /* buffer for dead trace prototype */
   char *diff1_buf;             /* trace buffer for diff1 debug dump */
   char *diff2_buf;             /* trace buffer for diff2 debug dump */
   int green_trace_min;         /* minimum green word size */
   int green_trace_max;         /* maximum green word size */

   /* "standard" trace flags and header linkage */
   union {
      DDS_BIN_USP usp;          /* iff usp format */
      DDS_BIN_SEGY segy;        /* iff segy format */
      DDS_BIN_GEN gen;          /* iff generic format */
   } std;
 
   DDS_DUMP_ITEM *dump_item;    /* dds_dump_max */
 
} DDS_BIN;
 
typedef struct DDS_MAP
{  /* "map" descriptor, unique combination (see DDS_ROOT.map) */
   struct DDS_MAP *next;        /* next out_type/in_type pair. */
                                /*    The list is anchored in the 2D */
                                /*    array dds_map[out_bin][in_bin]. */
   DDS_BIN *out_bin;            /* output binary descriptor */
   DDS_OBJECT *out_object;      /* lvalue object pointer. */
 
   DDS_BIN *in_bin;             /* input binary descriptor */
   DDS_OBJECT *in_object;       /* rvalue object pointer. */
 
   DDS_SEAM seam;               /* evaluated upon mapping */
} DDS_MAP;

/*****************************************/
/* extern global variables and constants */
/*****************************************/

extern DDS_BIN *dds_compile;    /* compile binary descriptor */
                                /*    The parser and scanner append */
                                /*    symbols to the tables */
                                /*    referenced by this DDS_BIN. */

/* Non-NULL iff compiling maps and input buffer type is struct/union. */
extern DDS_VALUE *dds_map_value;/* compiler handle to "in" struct. */

extern int dds_map_samples;     /* switch for cddx_outhdr to turn off mapping Samples */

extern unsigned dds_bin_max;    /* maximum number of DDS_BIN pointers */
extern DDS_BIN **dds_bin;       /* binary descriptor table (bin_tag) */
                                /*    bin[bin_max] */
extern DDS_MAP **dds_map;       /* map descriptor table (out/in bin_tag) */
                                /*    map[bin_max][bin_max] */

/***********************/
/* function prototypes */
/***********************/

int ddsOpenBin(
   DDS_BIN *bin,
   const char *open_format,
   const char *open_data)
;
int ddsReserveBin(
   const char *alias,
   const char *value,
   const char *title,
   const char *open_mode)
;
int ddsDelayedOpen(
   BIN_TAG bin_tag)
;
int ddsOpenNameTest(
   DDS_BIN *bin,
   DDS_STRING *data_name,
   int *slice_max)
;
int ddsOpenNameMake(
   DDS_BIN *bin,
   DDS_STRING *data_name)
;
int ddsOpenSlice(
   DDS_BIN *bin,
   const char *data_name,
   const char *open_format,
   DDS_CUBE *old_cube)
;
void ddsSetSliceLevel(
   const char *mode, 
   int adjust)
;
int ddsUncompileFmt(
   DDS_BIN *bin)
;
int ddsOpenBlock(
   DDS_BIN *bin)
;
void ddsOpenEstimate(
   DDS_BIN *bin)
;
int ddsOpenNext(
   DDS_BIN *bin,
   const char *mode)
;
int ddsOpenOld(
   DDS_BIN *bin,
   const char *data_name,
   DDS_CUBE *old_cube)
;
int ddsOpenNew(
   DDS_BIN *bin,
   const char *data_name,
   DDS_CUBE *old_cube)
;
void ddsOpenNewUsp(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
;
void ddsOpenOldUsp(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
;
void ddsOpenNewSegy(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
;
void ddsOpenOldSegy(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
;
void ddsOpenNewGen(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
;
void ddsOpenOldGen(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
;
DDS_MAP * ddsOpenMap(
   DDS_BIN *out_bin,
   DDS_TYPE *out_type,
   DDS_BIN *in_bin,
   DDS_TYPE *in_type)
;
void ddsPrintInfo(
   DDS_BIN *bin)
;
void ddsPrintCloseInfo(
   DDS_BIN *bin)
;
 
int ddsCompileFmt(
   DDS_BIN *bin,
   DDS_BOOLEAN create)
;
int ddsMapCompile(
   DDS_MAP *map)
;
int ddsMapCompileSU(
   DDS_MAP *map,
   DDS_STRING *out_prefix,
   long out_bias,
   DDS_TYPE *out_type,
   DDS_BOOLEAN map_default)
;
int ddsMapCompileSU2(
   DDS_MAP *map,
   const char *out_name,
   long out_bias,
   DDS_MEMBER *out_member,
   DDS_BOOLEAN map_default)
;
void ddsMapAssign(
   const char *expr_name,
   DDS_BIN *out_bin,
   DDS_OBJECT *out_object,
   const char *out_name,
   DDS_VALUE *rvalue,
   DDS_OBJECT *in_object,
   DDS_SEAM *seam)
;
void ddsMapOptimize(
   DDS_SEAM *seam,
   DDS_OBJECT *in_object)
;
 
 
int ddsReadGenT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buf,
   int count)
;
int ddsReadGen(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buf,
   int count)
;
int ddsReadUspL(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buf,
   int count)
;
int ddsReadUspT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buf,
   int count)
;
int ddsReadSegyC(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buffer,
   int count)
;
int ddsReadSegyB(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buffer,
   int count)
;
int ddsReadSegyT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buffer,
   int count)
;
int ddsReadSlice(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buffer,
   int count)
;
int ddsReadSliceT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buffer,
   int count)
;
int ddsReadCheck(
   DDS_BIN *bin,
   DDS_TYPE *type,
   void *buffer,
   int count,
   int n_read,
   int residual)
;
int ddsReadPreamble(
   DDS_BIN *bin)
;


int ddsWriteGenT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buf,
   int count)
;
int ddsWriteGen(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buf,
   int count)
;
int ddsWriteUspL(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buf,
   int count)
;
int ddsWriteUspT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buf,
   int count)
;
int ddsWriteSegyC(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buffer,
   int count)
;
int ddsWriteSegyB(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buffer,
   int count)
;
int ddsWriteSegyT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buffer,
   int count)
;
int ddsWriteSlice(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buffer,
   int count)
;
int ddsWriteSliceT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buffer,
   int count)
;
int ddsWriteCheck(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const void *buffer,
   int count,
   int n_read,
   int residual)
;
int ddsWritePreamble(
   DDS_BIN *bin)
;


long ddsGetOrdinal(
   DDS_COORD coord,
   DDS_AXIS *axis)
;
DDS_COORD ddsGetExpected(
   long ordinal,
   DDS_AXIS *axis)
;
void * ddsEditTrace(
   DDS_BIN *bin,
   DDS_TYPE *type,
   void *buffer,
   DDS_BIN_STATE msg_state)
;
void * ddsEditFinal(
   DDS_BIN *bin,
   DDS_TYPE *type,
   DDS_BIN_STATE msg_state)
;
void ddsCubeInc(
   DDS_BIN *bin)
;
 
 
HOST_INT6_T ddsSeekSlice(
   DDS_BIN *bin,
   DDS_TYPE *type,
   HOST_INT6_T offset,
   int whence)
;
HOST_INT6_T ddsSeekHeader(
   DDS_BIN *bin,
   DDS_TYPE *type,
   HOST_INT6_T offset,
   int whence)
;
HOST_INT6_T ddsSeekType(
   DDS_BIN *bin,
   DDS_TYPE *type,
   HOST_INT6_T offset,
   int whence)
;
HOST_INT6_T ddsSeekCur(
   DDS_BIN *bin,
   DDS_TYPE *type,
   HOST_INT6_T offset,
   HOST_LLONG_T *TrcOff)
;
HOST_INT6_T ddsSeekEnd(
   DDS_BIN *bin,
   DDS_TYPE *type,
   HOST_INT6_T offset,
   HOST_LLONG_T *TrcOff)
;
HOST_INT6_T ddsSeekSet(
   DDS_BIN *bin,
   DDS_TYPE *type,
   HOST_INT6_T offset,
   HOST_LLONG_T *TrcOff)
;


int ddsCloseBin(
   DDS_BIN *bin)
;
int ddsCloseFlush(
   DDS_BIN *bin)
;


void ddsBinMessage(
   DDS_BIN *bin,
   DDS_BIN_STATE setup)
;


int ddsLinkMember(
   DDS_MEMBER_REF *ref,
   DDS_BLOCK *block,
   const char *name)
;
int ddsBinFreeze(
   DDS_BIN *bin)
;
int ddsObjectTag(
   DDS_OBJECT_REF *ref,
   DDS_BLOCK *block,
   const char *name)
;
int ddsLinkObject(
   DDS_OBJECT_REF *ref)
;
int ddsLinkObjectSU(
   DDS_BIN *bin,
   DDS_OBJECT *object,
   DDS_SEAM *seam,
   const char *mode,
   DDS_STRING *out_prefix,
   long out_bias,
   DDS_TYPE *out_type)
;
int ddsLinkObjectSU2(
   DDS_BIN *bin,
   DDS_OBJECT *object,
   DDS_SEAM *seam,
   const char *mode,
   const char *out_name,
   long out_bias)
;
int ddsTraceTag(
   DDS_BIN *bin)
;

 
void ddsCubeLinker(
   DDS_BIN *bin)
;
void ddsCubePrint(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
;
void ddsCubeScanInit(
   DDS_BIN *bin)
;
void ddsCubeScanAxis(
   DDS_AXIS *axis,
   DDS_STRING *alias)
;
void ddsCubeScanBin(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
;
const char * ddsReformName(
   int rank,
   const char *format,
   const char *axis_name,
   const char *old_name,
   const char *old_format)
;


const char * ddsHintType(
   DDS_BIN *bin,
   DDS_TYPE *type)
;


int ddsReadFile(
   DDS_BIN *bin)
;
int ddsWriteFile(
   DDS_BIN *bin)
;

#endif /* __DDS_BINARY_H */
