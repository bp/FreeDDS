static char rcsid[]="$Id: trynontuned.c 13 2009-01-06 16:56:43Z user $";
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

   setup.title = "21: verify and test _IONON_TUNED (constant cache size)";
   /* Fixed buffering (block size > 0), with _IONON_TUNED,
      i.e. force constant cache size and inhibits dynamic tuning.
      The best case from these tests should be compared with
      the corresponding tuned buffering results.
      If tuned buffering is significantly worse, then something
      is wrong with big, max, or program logic.
   */
   setup.pass_max = 1;
   setup.size_max = 1;
   setup.size_list[0] = 8 * GIO_KB;
   setup.block_max = 8;
   setup.block_list[0] =       setup.blk_size;
   setup.block_list[1] =   2 * setup.blk_size;
   setup.block_list[2] =   4 * setup.blk_size;
   setup.block_list[3] =   8 * setup.blk_size;
   setup.block_list[4] =  16 * setup.blk_size;
   setup.block_list[5] =  32 * setup.blk_size;
   setup.block_list[6] =  64 * setup.blk_size;
   setup.block_list[7] = 128 * setup.blk_size;
   TM_mode = (TST_MASK)(TM_read_forward | TM_create_forward);
   TM_pack = (TST_MASK)(TM_buf_ptr | TM_map_ptr | TM_raw_ptr |
                        TM_buf_cpy | TM_map_cpy | TM_raw_cpy);
   setup.cache_flags = _IONON_TUNED;
   TM_form = TM_speed;
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
