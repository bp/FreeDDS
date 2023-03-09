static char rcsid[]="$Id: tstloopbias.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstLoopBias: fixed, random
   tstLoopBlock: system io block sizes
      tstTrial: conduction one test/tuneup trial.

*/

#include "tstsys.h"

void tstLoopBias(
   TST_SETUP *setup,
   int pass,
   long pick_size,
   int pick_align)
{
   TST_CASE TC_bias;
   int i, pick_bias;

   for(TC_bias = TC_bias_fixed;
      TC_bias <= TC_bias_random;
      TC_bias++) {

      if(! (setup->tune & 1 << TC_bias)) continue;

      if(TC_bias == TC_bias_random &&
         (setup->bias_max == 1 ||
            (setup->pack != TC_buf_ptr &&
             setup->pack != TC_map_ptr &&
             setup->pack != TC_raw_ptr))) continue;

      for(pick_bias = 0;
         pick_bias < setup->bias_max;
         pick_bias++) {

         if(TC_bias == TC_bias_fixed) {
            setup->bias_max2 = 1;
            setup->bias_list2 = &setup->bias_list[pick_bias];

            if(pass == setup->pass_max)
               fprintf(stderr, "      %s= %i\n",
                  bias_name[TC_bias - TC_bias_fixed],
                  setup->bias_list[pick_bias]);
         } else {
            setup->bias_list2 = &setup->bias_list[0];
            pick_bias = setup->bias_max2 = setup->bias_max;

            if(pass == setup->pass_max) {
               fprintf(stderr, "      %s= ",
                  bias_name[TC_bias - TC_bias_fixed]);

               for(i = 0; i < pick_bias; i++)
                  fprintf(stderr, "%i  ", setup->bias_list[i]);
               fprintf(stderr, "\n");
            }
         }

         tstLoopBlock(setup, pass, pick_size, pick_align, pick_bias);

      }
   }
   return;
}
