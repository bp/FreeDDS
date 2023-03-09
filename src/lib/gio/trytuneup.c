static char rcsid[]="$Id: trytuneup.c 13 2009-01-06 16:56:43Z user $";
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

   setup.title = "3: verify and find limits for big/max";
   /* Fixed buffering (block size > 0), with _IONON_TUNED,
      i.e. force constant cache size and inhibits dynamic tuning.

      A range of sizes (8 nominal) are tested.
      The idea is determine peak performance and
      some limits (big/max) for gsetvbuf usage.

      The "big" limit should be 80% to 95% of measured peak.
      The "max" limit should be 90% to 100% of peak.
      These results should be applied to "gsetvbuf.c".
      See struct GFILE in gio_types.h, specifically members
      big_r, max_r, big_w, max_w.
   */
   setup.pass_max = 2;
   setup.size_max = 1;
   setup.size_list[0] = setup.blk_size;
   setup.block_max = 14;
   setup.block_list[0] =       setup.blk_size;
   setup.block_list[1] =   2 * setup.blk_size;
   setup.block_list[2] =   4 * setup.blk_size;
   setup.block_list[3] =   8 * setup.blk_size;
   setup.block_list[4] =  16 * setup.blk_size;
   setup.block_list[5] =  32 * setup.blk_size;
   setup.block_list[6] =  64 * setup.blk_size;
   setup.block_list[7] = 128 * setup.blk_size;
   setup.block_list[8]  =  15625;
   setup.block_list[9]  =  31250;
   setup.block_list[10] =  62500;
   setup.block_list[11] = 125000;
   setup.block_list[12] = 250000;
   setup.block_list[13] = 500000;

   setup.cache_flags = _IONON_TUNED;
   TM_mode = (TST_MASK)(TM_read_forward | TM_create_forward);
   TM_pack = (TST_MASK)(TM_buf_ptr | TM_buf_cpy |
                        TM_map_ptr | TM_map_cpy);
setup.pass_max = 1;
/***
setup.size_list[0] = 4800;
setup.block_max = 6;
setup.block_list[0] =  15625;
setup.block_list[1] =  31250;
setup.block_list[2] =  62500;
setup.block_list[3] = 125000;
setup.block_list[4] = 250000;
setup.block_list[5] = 500000;
***/
TM_mode = (TST_MASK)(TM_read_forward);

TM_pack = (TST_MASK)(TM_buf_ptr | TM_buf_cpy);

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
