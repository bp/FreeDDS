static char rcsid[]="$Id: tryverifybig4.c 13 2009-01-06 16:56:43Z user $";
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

   setup.title = "10: verify accuracy, big 4 fixed combinations";
   /* similar to previous test, with fewer combinations,
      i.e. it has the chance to run to completion.
      The size, align, and bias values are fixed (constant).
      Eliminated trials are read_backward, write_forward,
      write_backward.
      Only two sizes (drop 48, keep 4800, 480000) are tested.
   */
   setup.pass_max = 2;
   setup.size_max = 2;
   setup.size_list[0] = 4800;
   setup.size_list[1] = 480000;
   setup.block_max = 2;
   setup.block_list[0] = -1;
   setup.block_list[1] =  0;
   TM_mode = (TST_MASK)(TM_read_forward | TM_read_random |
                        TM_write_random | TM_create_forward);
   TM_pack = (TST_MASK)(TM_stdio | TM_usp |
                        TM_buf_ptr | TM_buf_cpy |
                        TM_map_ptr | TM_map_cpy |
                        TM_raw_ptr | TM_raw_cpy);
TM_pack = (TST_MASK)(TM_usp);
setup.block_max = 1;
setup.block_list[0] =  0;
TM_mode = (TST_MASK)(TM_read_forward);
setup.pass_max = 3;
/***
setup.pass_max = 1;
TM_pack = (TST_MASK)(TM_buf_ptr | TM_buf_cpy |
                     TM_map_ptr | TM_map_cpy |
                     TM_raw_ptr | TM_raw_cpy);
TM_mode = (TST_MASK)(TM_read_random);
TM_mode = (TST_MASK)(TM_write_random);
***/
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
