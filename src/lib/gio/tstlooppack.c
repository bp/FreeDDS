static char rcsid[]="$Id: tstlooppack.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstLoopPack: stdio, usp, gio (buf/map/raw and ptr/cpy)
   tstLoopMode: read/write/create and foward/backward/random
      tstLoopSize: fixed, random
         tstLoopAlign: fixed, random
            tstLoopBias: fixed, random
               tstLoopBlock: system io block sizes
                  tstTrial: conduction one test/tuneup trial.

*/

#include "tstsys.h"

void tstLoopPack(
   TST_SETUP *setup,
   int pass)
{
   for(setup->pack = TC_stdio;
      setup->pack <= TC_raw_cpy;
      setup->pack++) {
      if(! (setup->tune & 1 << setup->pack)) continue;
      #ifndef GIO_MMAP_IS_SUPPORTED
         if(setup->pack == TC_map_cpy) continue;
         if(setup->pack == TC_map_ptr) continue;
      #endif /* GIO_MMAP_IS_SUPPORTED */
      #ifndef GIO_RAWIO_IS_SUPPORTED
         if(setup->pack == TC_raw_cpy) continue;
         if(setup->pack == TC_raw_ptr) continue;
      #endif /* GIO_RAWIO_IS_SUPPORTED */
      #ifndef TST_USP
         if(setup->pack == TC_usp) continue;
      #endif /* TST_USP */
      #ifndef TST_VDS
         if(setup->pack == TC_vds) continue;
      #endif /* TST_VDS */
      if(pass == setup->pass_max) {
         fprintf(stderr, "\n*********************************\n");
         fprintf(stderr, "******  %s %s  ******\n",
            form_name[setup->form - TC_verify],
            pack_name[setup->pack - TC_stdio]);
         fprintf(stderr, "***********************************\n\n");
      }

      tstLoopMode(setup, pass);

   }

   return;
}
