static char rcsid[]="$Id: tryoverhead.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tryOverhead: overhead without any trial io.

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

   /* Crank through all the loops, without *any* trial i/o. */
   /* This measures the overhead of the test program itself. */
   setup.title = "0: overhead test";
   TM_form = TM_speed;
   TM_mode = TM_nop;

   setup.time_limit = 10.;
   setup.file_limit = 1 * 1000000;

   /* Save the set of trials to be performed. */
   setup.tune = (TST_MASK)(TM_form | TM_pack | TM_mode |
      TM_size | TM_align | TM_bias);

   /* This loops over selected combinations. */
   tstLoopPass(&setup);

   /* cleanup the test files, which may be big. */
   remove(setup.filename);

   exit(0);
}
