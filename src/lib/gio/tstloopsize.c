static char rcsid[]="$Id: tstloopsize.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstLoopSize: fixed, random
   tstLoopAlign: fixed, random
      tstLoopBias: fixed, random
         tstLoopBlock: system io block sizes
            tstTrial: conduction one test/tuneup trial.

*/

#include "tstsys.h"

void tstLoopSize(
   TST_SETUP *setup,
   int pass)
{
   TST_CASE TC_size;
   long i, pick_size, usp_bytes;

   for(TC_size = TC_size_fixed;
      TC_size <= TC_size_random;
      TC_size++) {

      if(! (setup->tune & 1 << TC_size)) continue;

      if(TC_size == TC_size_random &&
         (setup->size_max == 1 ||
          setup->pack == TC_usp ||
          setup->pack == TC_vds)) continue;

      for(pick_size = 0;
         pick_size < setup->size_max;
         pick_size++) {

         if(TC_size == TC_size_fixed) {
            setup->size_max2 = 1;
            setup->size_list2 =
               &setup->size_list[pick_size];

            #ifdef TST_USP
               if(setup->pack == TC_usp) {
                  if(*setup->size_list2 > SZDTHD + 9000 * sizeof(float) ||
                     *setup->size_list2 < SZDTHD + sizeof(float))
                     continue;

                  setup->usp_samples = (*setup->size_list2 -
                     SZDTHD) / sizeof(float);
                  usp_bytes = SZDTHD + setup->usp_samples * sizeof(float);
                  setup->size_list2 = &usp_bytes;
                  setup->usp_traces = setup->file_limit / usp_bytes;
               }
            #endif /* TST_USP */

            if(pass == setup->pass_max)
               fprintf(stderr, "  %s= %i\n",
                  size_name[TC_size - TC_size_fixed],
                  *setup->size_list2);
         } else {
            setup->size_list2 = &setup->size_list[0];
            pick_size =
               setup->size_max2 = setup->size_max;

            if(pass == setup->pass_max) {
               fprintf(stderr, "  %s= ",
                  size_name[TC_size - TC_size_fixed]);
               for(i = 0; i < pick_size; i++)
                  fprintf(stderr, "%i  ",
                     setup->size_list[i]);
               fprintf(stderr, "\n");
            }
         }

         tstLoopAlign(setup, pass, pick_size);

      }
   }
   return;
}
