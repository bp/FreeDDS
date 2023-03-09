static char rcsid[]="$Id: tstloopmode.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstLoopMode: read/write/create and foward/backward/random
   tstLoopSize: fixed, random
      tstLoopAlign: fixed, random
         tstLoopBias: fixed, random
            tstLoopBlock: system io block sizes
               tstTrial: conduction one test/tuneup trial.

*/

#include "tstsys.h"

void tstLoopMode(
   TST_SETUP *setup,
   int pass)
{
   for(setup->mode = TC_nop;
      setup->mode <= TC_create_forward;
      setup->mode++) {

      if(! (setup->tune & 1 << setup->mode)) continue;

      if(setup->mode == TC_nop && setup->pack != TC_stdio) continue;

      switch(setup->pack) {
      case TC_vds:
         if(setup->mode == TC_write_forward ||
            setup->mode == TC_write_backward ||
            setup->mode == TC_write_random) continue;
         break;
      case TC_map_cpy:
      case TC_map_ptr:
         if(setup->mode >= TC_write_forward) continue;
         break;
      case TC_raw_cpy:
      case TC_raw_ptr:
         if(setup->mode >= TC_write_forward) continue;
         break;
      }

      if(pass == setup->pass_max)
         fprintf(stderr, "%s\n",
            mode_name[setup->mode - TC_nop]);

      tstLoopSize(setup, pass);

   }
   return;
}
