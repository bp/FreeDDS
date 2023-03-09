static char rcsid[]="$Id: tryverifyall.c 13 2009-01-06 16:56:43Z user $";
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

   setup.title = "5: verify accuracy, all combinations";
   /* This test is horrible long (days, or weeks for one repeat !!!)
      because of all the combinations.
      It includes fixed and variable size/align/bias.
      The ideas is to exhaustively verify all combinations.
   */
   setup.pass_max = 1;
   setup.size_max = 3;
   setup.size_list[0] = 48;
   setup.size_list[1] = 4800;
   setup.size_list[2] = 480000;
   setup.block_max = 2;
   setup.block_list[0] = -1;
   setup.block_list[1] =  0;
   TM_mode = (TST_MASK)(
      TM_read_forward  | TM_read_backward  | TM_read_random |
      TM_write_forward | TM_write_backward | TM_write_random |
      TM_create_forward);
   TM_pack = (TST_MASK)(
      TM_stdio   | TM_usp |
      TM_buf_ptr | TM_buf_cpy |
      TM_map_ptr | TM_map_cpy |
      TM_raw_ptr | TM_raw_cpy);
   TM_form = (TST_MASK)(TM_verify);
   TM_size = (TST_MASK)(TM_size_fixed | TM_size_random);
   TM_align = (TST_MASK)(TM_align_fixed | TM_align_random);
   TM_bias = (TST_MASK)(TM_bias_fixed | TM_bias_random);

   /* Save the set of trials to be performed. */
   setup.tune = (TST_MASK)(TM_form | TM_pack | TM_mode |
      TM_size | TM_align | TM_bias);

   /* This loops over selected combinations. */
   tstLoopPass(&setup);

   /* cleanup the test files, which may be big. */
   remove(setup.filename);

   exit(0);
}
