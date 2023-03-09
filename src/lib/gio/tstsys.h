/*
RCS-t=GIO, tstsys.h: test system header file.

$Id: tstsys.h 13 2009-01-06 16:56:43Z user $
*/

#include <stdio.h>
#include <time.h>
#include "gio_sys.h"

#ifdef TST_USP
   #include "size_defs.h"
   #include "save_defs.h"
   #include "io_defs.h"
#endif /* TST_USP */

#define TST_SIZE_MAX    13
#define TST_ALIGN_MAX    7
#define TST_BIAS_MAX     1
#define TST_BLOCK_MAX   14

#define INIT_RATIO 500

typedef enum
{  /* list of test cases and combinations */
   TC_verify,          /* verify each read within test loop */
   TC_speed,           /* speed test (don't verify reads)   */

   TC_stdio,           /* package stdio package                  */
   TC_usp,             /* package usp                            */
   TC_vds,             /* package disco                          */
   TC_buf_ptr,         /* package GIO, buffer  (pointer passing) */
   TC_buf_cpy,         /* package GIO, buffer  (copy data)       */
   TC_map_ptr,         /* package GIO, mem map (pointer passing) */
   TC_map_cpy,         /* package GIO, mem map (copy data)       */
   TC_raw_ptr,         /* package GIO, raw i/o (pointer passing) */
   TC_raw_cpy,         /* package GIO, raw i/o (copy data)       */

   TC_nop,             /* no operation, measure overhead without io */
   TC_read_forward,    /* sequential, repeating                     */
   TC_read_backward,   /* back-to-front sequential, repeating       */
   TC_read_random,     /* offset = align * random + bias            */
   TC_write_forward,   /* sequential, repeating                     */
   TC_write_backward,  /* back-to-front sequential, repeating       */
   TC_write_random,    /* offset = align * random + bias            */
   TC_create_forward,  /* sequential, create once only              */

   TC_size_fixed,      /* constant size (size_list) */
   TC_size_random,     /* random size (size_list)   */

   TC_align_fixed,     /* constant alignment (align_list) */
   TC_align_random,    /* random alignment (align_list)   */

   TC_bias_fixed,      /* constant bias (bias_list) */
   TC_bias_random      /* random bias (bias_list)   */
} TST_CASE;

typedef enum
{  /* mask for each test option */
   TM_verify          = 1 << TC_verify,
   TM_speed           = 1 << TC_speed,

   TM_stdio           = 1 << TC_stdio,
   TM_usp             = 1 << TC_usp,
   TM_vds             = 1 << TC_vds,
   TM_buf_ptr         = 1 << TC_buf_ptr,
   TM_buf_cpy         = 1 << TC_buf_cpy,
   TM_map_ptr         = 1 << TC_map_ptr,
   TM_map_cpy         = 1 << TC_map_cpy,
   TM_raw_ptr         = 1 << TC_raw_ptr,
   TM_raw_cpy         = 1 << TC_raw_cpy,

   TM_nop             = 1 << TC_nop,
   TM_read_forward    = 1 << TC_read_forward,
   TM_read_backward   = 1 << TC_read_backward,
   TM_read_random     = 1 << TC_read_random,
   TM_write_forward   = 1 << TC_write_forward,
   TM_write_backward  = 1 << TC_write_backward,
   TM_write_random    = 1 << TC_write_random,
   TM_create_forward  = 1 << TC_create_forward,

   TM_size_fixed      = 1 << TC_size_fixed,
   TM_size_random     = 1 << TC_size_random,

   TM_align_fixed     = 1 << TC_align_fixed,
   TM_align_random    = 1 << TC_align_random,

   TM_bias_fixed      = 1 << TC_bias_fixed,
   TM_bias_random     = 1 << TC_bias_random
} TST_MASK;

typedef struct TST_SCORE
{  /* test summary, accumulated for final report */
   double elapsed;             /* elapsed time in seconds. */
   double elapsed_mbs;         /* mb/sec elapsed time. */
   double cpu;                 /* cpu time in seconds. */
   double cpu_mbs;             /* mb/sec cpu time. */
   double mb;                  /* total megabytes transfered. */
   long loop;                  /* total number of read/writes. */
} TST_SCORE;

/* score board for each combination and any pass. */
typedef TST_SCORE TST_BOARD
   [TC_speed          - TC_verify + 1]
   [TC_raw_cpy        - TC_stdio  + 1]
   [TC_create_forward - TC_nop    + 1]
   [TST_SIZE_MAX  + 1]
   [TST_ALIGN_MAX + 1]
   [TST_BIAS_MAX  + 1]
   [TST_BLOCK_MAX + 1];

typedef struct TST_SETUP
{  /* General setup parameters for test processing. */
   const char *hostname;       /* host name */
   const char *filename;       /* filename for i/o test */
   long blk_size;              /* nominal system io block size (bytes) */

   double read_mbs;            /* anticipated stdio read  mb/sec */
   double write_mbs;           /* anticipated stdio write mb/sec */

   const char *title;          /* test case title */
   int smart;                  /* skip silly test cases */
   int watch;                  /* cache watch (flag and loop maximum) */
   int verbose;                /* verbose report */
   unsigned seed;              /* random number seed */
   double time_limit;          /* minimum seconds per trial */
                               /*    Sufficient to read or write the */
                               /*    entire file at least once, with */
                               /*    min of 1 minute, max of 2. */
   HOST_LLONG_T file_limit;    /* maximum bytes per trial file */
   long cache_limit;           /* file system cache size (megabytes) */
                               /*    After initializing the test file */
                               /*    and before a read speed trial */
                               /*    another file is written to flush */
                               /*    test data from the system cache. */
                               /*    This prevents cached data from */
                               /*    obscuring the actually read speed. */
                               /*    Note: sync is not sufficient */
                               /*    because the cache may remain valid. */
                               /*    cache_limit is the flush size and */
                               /*    the minimum size for file_limit. */
                               /*    The cache_limit maximum is the size */
                               /*    of the real memory, or less if an */
                               /*    upper limit is known for the system. */
   int cache_flags;            /* gsetvbuf cache flags actually used. */

   int pass_max;               /* maximum number of passes */
                               /*    0 == report format check only */
   int trial_count;            /* trial serial number */

   TST_MASK tune;              /* test combinations (selected) */
   TST_CASE form;              /* trial verify or speed */
   TST_CASE pack;              /* trial package (stdio, usp, gio, etc.) */
   TST_CASE mode;              /* trial read and write pattern */

   int size_max;               /* setup number of sizes */
   long size_list[TST_SIZE_MAX];    /* API io size (bytes) */
   int size_max2;              /* trial number of sizes */ 
   long *size_list2;           /* trial API io size(s) */

   int align_max;              /* setup number of align values */
   int align_list[TST_ALIGN_MAX];  /* API io buffer alignment (bytes) */
   int align_max2;             /* trial number of sizes */ 
   int *align_list2;           /* trial API io align(s) */

   int bias_max;               /* setup number of bias values */
   int bias_list[TST_BIAS_MAX];    /* API io buffer bias (bytes)*/
   int bias_max2;              /* trial number of biases */ 
   int *bias_list2;            /* trial API io bias(s) */

   int block_max;              /* setup number of block values */
   long block_list[TST_BLOCK_MAX];  /* system io block size (bytes) */
   long block;                 /* trial block size (bytes) */
                               /*    -1 = unbuffered */
                               /*     0 = default block size */
                               /*     1 = special case */
                               /*     N = explicit block size */

   TST_BOARD *board;           /* score board */

   /* Trial specific setup paramaters for test processing. */

   char *trial;                /* sub-title for trial */

   long loop;                  /* loop counter for read/write/seek */
   long loop_max;              /* maximum loop count, fail safe */

   int rc;                     /* last return code */
   int error;                  /* error code */
   long short_read;            /* short read counter (problem ?) */
   int usp_samples;            /* number of samples per trace */
   int usp_traces;             /* number of traces per record */

   double elapsed;             /* elapsed time (seconds) */
   double cpu;                 /* accumulated cpu (seconds) */
   double read_mb;             /* read  accumulated mb */
   double write_mb;            /* write accumulated mb */

   long read;                  /* API read  count */
   long write;                 /* API write count */
   long seek;                  /* API seeks count (random/EOF/BOF) */

   HOST_LLONG_T read_bytes;    /* API read  byte count */
   HOST_LLONG_T write_bytes;   /* API write byte count */

/*** deprecated debug junk
   long user_read;
   long user_write;
   long user_seek;

   long system_read;
   long system_write;
   long system_seek;

   double user_read_mb;
   double user_write_mb;

   double system_read_mb;
   double system_write_mb;
***/
} TST_SETUP;

/* mnuemonics for enumerated types. */
extern const char *form_name[];
extern const char *pack_name[];
extern const char *mode_name[];
extern const char *size_name[];
extern const char *align_name[];
extern const char *bias_name[];

int tstClose(
   TST_SETUP *setup,
   char *tmp_file,
   FILE *fp_clear,
   FILE *fp_stdio,
   GFILE *fp_gio,
   int lu_usp,
   char *cbuf,
   unsigned *ubuf)
;
int tstExecute(
   TST_SETUP *setup,    /* tuner descriptor */
   long size_limit,      /* maximum size */
   int align_limit,      /* maximum align */
   int bias_limit,       /* maximum bias */
   FILE *fp_stdio,       /* stdio file pointer */
   GFILE *fp_gio,        /* GIO file pointer */
   int lu_usp,           /* logical unit usp */
   char *cbuf,           /* copy i/o buffer */
   unsigned *ubuf)       /* verify buffer */
;
int tstInit(
   TST_SETUP *setup,
   FILE **fp_clear,
   char *tmp_file)
;
void tstLoopAlign(
   TST_SETUP *setup,
   int pass,
   long pick_size)
;
void tstLoopBias(
   TST_SETUP *setup,
   int pass,
   long pick_size,
   int pick_align)
;
void tstLoopBlock(
   TST_SETUP *setup,
   int pass,
   long pick_size,
   int pick_align,
   int pick_bias)
;
void tstLoopGoal(
   TST_SETUP *setup,
   int pass)
;
void tstLoopMode(
   TST_SETUP *setup,
   int pass)
;
void tstLoopPack(
   TST_SETUP *setup,
   int pass)
;
void tstLoopPass(
   TST_SETUP *setup)
;
void tstLoopSize(
   TST_SETUP *setup,
   int pass)
;
int tstOpen(
   TST_SETUP *setup,
   FILE **fp_stdio,
   GFILE **fp_gio,
   int *lu_usp,
   char **cbuf,
   unsigned **ubuf,
   long size_limit)
;
void tstScore(
   TST_SETUP *setup)
;
int tstSetup(          /* status (non-zero == error). */
   TST_SETUP *setup,   /* setup for one particular trial. */
   long *size_limit,
   int *align_limit,
   int *bias_limit)
;
void tstSystem(
   TST_SETUP *setup,
   int argc,
   const char** argv)
;
void tstSummary(
   TST_SETUP *setup)
;
int tstTrial(           /* status (non-zero == error). */
   TST_SETUP *setup)   /* setup for one particular trial. */
;
int tstVerify(
   TST_SETUP *setup)
;
void *safemalloc(
   size_t)
;
