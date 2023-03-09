static char rcsid[]="$Id: tstloopgoal.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstLoopGoal: verify, speed
   tstLoopPack: stdio, usp, gio (buf/map/raw and ptr/cpy)
      tstLoopMode: read/write/create and foward/backward/random
         tstLoopSize: fixed, random
            tstLoopAlign: fixed, random
               tstLoopBias: fixed, random
                  tstLoopBlock: system io block sizes
                     tstTrial: conduction one test/tuneup trial.

*/

#include "tstsys.h"

void tstLoopGoal(
   TST_SETUP *setup,
   int pass)
{
   for(setup->form = TC_verify;
      setup->form <= TC_speed;
      setup->form++) {

      if(! (setup->tune & 1 << setup->form)) continue;

      tstLoopPack(setup, pass);

   }

   return;
}
