static char rcsid[]="$Id: tstloopalign.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstLoopAlign: fixed, random
   tstLoopBias: fixed, random
      tstLoopBlock: system io block sizes
         tstTrial: conduction one test/tuneup trial.

*/

#include "tstsys.h"

void tstLoopAlign(
   TST_SETUP *setup,
   int pass,
   long pick_size)
{
   TST_CASE TC_align;
   int i, pick_align;

   for(TC_align = TC_align_fixed;
      TC_align <= TC_align_random;
      TC_align++) {

      if(! (setup->tune & 1 << TC_align)) continue;

      if(TC_align == TC_align_random &&
         (setup->align_max == 1 ||
            (setup->pack != TC_buf_ptr &&
             setup->pack != TC_map_ptr &&
             setup->pack != TC_raw_ptr))) continue;

      for(pick_align = 0;
         pick_align < setup->align_max;
         pick_align++) {

         if(TC_align == TC_align_fixed) {
            setup->align_max2 = 1;
            setup->align_list2 = &setup->align_list[pick_align];

            if(pass == setup->pass_max)
               fprintf(stderr, "    %s= %i\n",
                  align_name[TC_align - TC_align_fixed],
                  setup->align_list[pick_align]);
         } else {
            setup->align_list2 = &setup->align_list[0];
            pick_align = setup->align_max2 = setup->align_max;

            if(pass == setup->pass_max) {
               fprintf(stderr, "    %s= ",
                  align_name[TC_align - TC_align_fixed]);

               for(i = 0; i < pick_align; i++)
                  fprintf(stderr, "%i  ", setup->align_list[i]);
               fprintf(stderr, "\n");
            }
         }

         tstLoopBias(setup, pass, pick_size, pick_align);

      }
   }
   return;
}
