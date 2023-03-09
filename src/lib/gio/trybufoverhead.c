static char rcsid[]="$Id: trybufoverhead.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, trySummary: report format, without any trials.

*/

#include "tstsys.h"

static TST_SETUP setup;

int main(
   int argc,
   const char** argv)
{
   TST_MASK TM_form  = (TST_MASK)-1,
            TM_pack  = (TST_MASK)-1,
            TM_mode  = (TST_MASK)-1,
            TM_size  = (TST_MASK)-1,
            TM_align = (TST_MASK)-1,
            TM_bias  = (TST_MASK)-1;

   /* Initialize test system with reasonable defaults. */
   tstSystem(&setup, argc, argv);

   setup.title = "2: verify and _IONON_CACHE comparison";
   /* Non-buffered (block size == -1) has simple logic.
      Default buffering (block size == 0), with _IONON_CACHE
      (force constant cache size == 0) is degenerate.
      ************************************************************
      ***  These two block sizes should yield similar speed.   ***
      ***  If not, then the slow version has excess overhead.  ***
      ************************************************************

      Relative to non-buffered:
         "buf_ptr" is sometimes faster, because ???
         "buf_cpy" is slightly slower, because memcpy is required.
   */
   setup.size_max = 1;
   setup.size_list[0] = 4800;
   setup.block_max = 2;
   setup.block_list[0] = -1;
   setup.block_list[1] = 0;
   setup.cache_flags = _IONON_CACHE;
   TM_mode = (TST_MASK)(TM_read_forward | TM_read_random
                    | TM_create_forward);
   /* exclude "map" and "raw", because non-buffered not supported. */
   TM_pack = (TST_MASK)(TM_buf_ptr | TM_buf_cpy);
   /* verify is used, instead of speed, because its more realistic. */
   /* speed is useful for a hyper critical overhead test. */
   TM_form = TM_verify;
   TM_size = TM_size_fixed;
   TM_align = TM_align_fixed;
   TM_bias = TM_bias_fixed;

   /* Save the set of trials to be performed. */
   setup.tune = (TST_MASK)(TM_form | TM_pack | TM_mode |
      TM_size | TM_align | TM_bias);

   /* This loops over selected combinations. */
   tstLoopPass(&setup);

   /* cleanup the test files, which may be big. */
   remove(setup.filename);

   exit(0);
}
